#include "PicQueue.h"
//
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
//
#include "common.h"


/////////////////////////////////////////////////////////////////////////
CPicQueue::CPicQueue(int iMaxPicCnt)
{
    pthread_mutexattr_t attr;

    m_iMaxPicCnt = iMaxPicCnt;

    m_cuPics = new CUVIDPARSERDISPINFO[iMaxPicCnt];
    m_iPicCnt = 0;
    m_iPicsUseStatus = new int[iMaxPicCnt];
    memset(m_iPicsUseStatus, 0x00, iMaxPicCnt * sizeof(int));
    m_iReadPos = 0;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_lkPics, &attr);
    pthread_mutexattr_destroy(&attr);

    m_termflag = 0;
}

CPicQueue::~CPicQueue()
{
    pthread_mutex_destroy(&m_lkPics);
}

bool CPicQueue::EnQueue(CUVIDPARSERDISPINFO *pcuPic)
{
    bool ret = false;
    int iWritePos;

    assert(pcuPic != NULL);

    while (1) {
        Lock();
        if (m_termflag) {
            Unlock();
            break;
        }

        if (m_iPicCnt >= m_iMaxPicCnt) {
            Unlock();
            usleep(5000);
            continue;
        }

        iWritePos = (m_iReadPos + m_iPicCnt) % m_iMaxPicCnt;
        m_iPicsUseStatus[pcuPic->picture_index] = 1;
        m_cuPics[iWritePos] = *pcuPic;
        m_iPicCnt++;

        Unlock();

        ret = true;
        break;
    }

    return ret;
}

bool CPicQueue::DeQueue(CUVIDPARSERDISPINFO **ppcuPic)
{
    assert(ppcuPic != NULL);

    Lock();

    if (m_iPicCnt <= 0) {
        Unlock();
        return false;
    }

    *ppcuPic = &m_cuPics[m_iReadPos];
    m_iPicCnt--;
    m_iReadPos = (m_iReadPos + 1) % m_iMaxPicCnt;

    Unlock();

    return true;
}

void CPicQueue::ReleasePic(CUVIDPARSERDISPINFO *pcuPic)
{
    assert(pcuPic != NULL);

    Lock();
    m_iPicsUseStatus[pcuPic->picture_index] = 0;
    Unlock();
}

bool CPicQueue::WaitUntilPicAvailable(int iPicIdx)
{
    int ret = false;

    assert(iPicIdx >= 0 && iPicIdx < m_iMaxPicCnt);

    while (1) {
        Lock();
        if (m_termflag) {
            Unlock();
            break;
        }

        if (m_iPicsUseStatus[iPicIdx]) {
            Unlock();
            usleep(5000);
            continue;
        }
        Unlock();
        ret = true;
        break;
    }

    return ret;
}

void CPicQueue::SetTermFlag()
{
    Lock();
    m_termflag = 1;
    Unlock();
}

void CPicQueue::Lock()
{
    pthread_mutex_lock(&m_lkPics);
}

void CPicQueue::Unlock()
{
    pthread_mutex_unlock(&m_lkPics);
}

