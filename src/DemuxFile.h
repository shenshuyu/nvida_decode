#ifndef DEMUX_FILE_H
#define DEMUX_FILE_H
#include <memory>
#include <iostream>
#include <string>
#include "string.h"

namespace AvDemuxFile {

typedef enum {
    AV_TYPE_H264,
    AV_TYPE_H265,
    AV_TYPE_G711,
    AV_TYPE_AAC,
} av_type_e;

class BaseAvFile
{
public:
    BaseAvFile(int bufSize=500000);
    virtual ~BaseAvFile();

    virtual bool open(const char *path);
    virtual void close();

    virtual av_type_e getAvType() = 0;
    
    bool isOpened() const
    {
        return (m_file != NULL);
    }

    virtual int readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame) = 0;
    
protected:
    FILE *m_file = NULL;
    char *m_buf = NULL;
    int m_bufSize = 0;
    int m_bytesUsed = 0;
    int m_count = 0;
};

class H264File: public BaseAvFile 
{
public:
    av_type_e getAvType() {return AV_TYPE_H264;}
    int readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame);
};

class H265File: public BaseAvFile
{
public:
    av_type_e getAvType() {return AV_TYPE_H265;}
    int readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame);
};

class G711File: public BaseAvFile 
{
private:
    uint32_t    m_length = 0;
    uint8_t *   m_filePointer = nullptr;
    int32_t     m_fileOffset = 0;
public:
    av_type_e getAvType() {return AV_TYPE_G711;}
    bool open(const char *path);
    void close();
    int readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame);
};

class AacFile: public BaseAvFile 
{
private:
    uint32_t    m_length = 0;
    uint8_t *   m_filePointer = nullptr;
    int32_t     m_fileOffset = 0;
public:
    av_type_e getAvType() {return AV_TYPE_AAC;}
    bool open(const char *path);
    void close();
    int readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame);
};
}
#endif