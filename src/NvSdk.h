/*************************************************************************
** file: NvSdk.h
** desc:
*************************************************************************/
#ifndef _NVSDK_H
#define _NVSDK_H
//
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
class CNvSdk
{
public:
    CNvSdk();
    ~CNvSdk();

private:
    bool LoadSdk();

public:
    bool    m_bLoadFlag;
    int     m_devnum;
};

extern CNvSdk gNvSdk;

#endif
