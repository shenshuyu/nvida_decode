/*************************************************************************
** file: PicQueue.h
** desc:
*************************************************************************/
#ifndef _PICQUEUE_H
#define _PICQUEUE_H
//
#include <pthread.h>
#include <stdint.h>
//
#include "dynlink_nvcuvid.h"

////////////////////////////////////////////////////////////////////////////////
class CPicQueue
{
public:
    CPicQueue(int iMaxPicCnt);
    ~CPicQueue();

    bool EnQueue(CUVIDPARSERDISPINFO *pcuPic);
    bool DeQueue(CUVIDPARSERDISPINFO **ppcuPic);
    void ReleasePic(CUVIDPARSERDISPINFO *pcuPic);

    bool WaitUntilPicAvailable(int iPicIdx);
    void SetTermFlag();

private:
    void Lock();
    void Unlock();

private:
    int                     m_iMaxPicCnt;

    CUVIDPARSERDISPINFO*    m_cuPics;
    int                     m_iPicCnt;
    int*                    m_iPicsUseStatus; /* 1use, 0nouse */
    int                     m_iReadPos;
    pthread_mutex_t         m_lkPics;

    int                     m_termflag;
};

#endif

