#include "DemuxFile.h"

using namespace AvDemuxFile;

BaseAvFile::BaseAvFile(int bufSize)
    : m_bufSize(bufSize)
{
    m_buf = new char[m_bufSize];
}

BaseAvFile::~BaseAvFile()
{
    delete m_buf;
}

bool BaseAvFile::open(const char *path)
{
    m_file = fopen(path, "rb");
    if(m_file == NULL)
    {      
       return false;
    }

    return true;
}

void BaseAvFile::close()
{
    if(m_file)
    {
        fclose(m_file);
        m_file = NULL;
        m_count = 0;
        m_bytesUsed = 0;
    }
}

int H265File::readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame)
{
    if(m_file == NULL)
    {
        return -1;
    }

    int bytesRead = fread(m_buf, 1, m_bufSize, m_file);
    if(bytesRead == 0)
    {
        fseek(m_file, 0, SEEK_SET); 
        m_count = 0;
        m_bytesUsed = 0;
        bytesRead = fread(m_buf, 1, m_bufSize, m_file);
        if(bytesRead == 0)        
        {            
            this->close();
            return -1;
        }
    }

    bool bFindStart = false, bFindEnd = false;

    int i = 0, startCode = 3;
    *bEndOfFrame = false;
    for (i=0; i<bytesRead-5; i++)
    {
        if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 1)
        {
            startCode = 3;
        }
        else if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 0 && m_buf[i+3] == 1)
        {
            startCode = 4;
        }
        else 
        {
            continue;
        }
        if (((m_buf[i+startCode]&0x1F) == 0x40 || (m_buf[i+startCode]&0x1F) == 0x42 
            || (m_buf[i+startCode]&0x1F) == 0x44 || (m_buf[i+startCode]&0x1F) == 0x4e
            || (m_buf[i+startCode]&0x1F) == 0x26 || (m_buf[i+startCode]&0x1F) == 0x2))// && ((m_buf[i+startCode+1]&0x80) == 0x80))              
        {
            bFindStart = true;
            i += 4;
            break;
        }
    }

    for (; i<bytesRead-5; i++)
    {
        if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 1)
        {
            startCode = 3;
        }
        else if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 0 && m_buf[i+3] == 1)
        {
            startCode = 4;
        }
        else 
        {
            continue;
        }
        if ((m_buf[i+startCode]&0x1F) == 0x40 || (m_buf[i+startCode]&0x1F) == 0x42 
            || (m_buf[i+startCode]&0x1F) == 0x44 || (m_buf[i+startCode]&0x1F) == 0x4e
            || (m_buf[i+startCode]&0x1F) == 0x26 || (m_buf[i+startCode]&0x1F) == 0x2)  // &&((m_buf[i+startCode+1]&0x80) == 0x80)))
        {
            bFindEnd = true;
            break;
        }
    }

    bool flag = false;
    if(bFindStart && !bFindEnd && m_count>0)
    {        
        flag = bFindEnd = true;
        i = bytesRead;
        *bEndOfFrame = true;
    }

    if(!bFindStart || !bFindEnd)
    {
        this->close();
        return -1;
    }

    int size = (i<=inBufSize ? i : inBufSize);
    memcpy(inBuf, m_buf, size); 

    if(!flag)
    {
        m_count += 1;
        m_bytesUsed += i;
    }
    else
    {
        m_count = 0;
        m_bytesUsed = 0;
    }

    fseek(m_file, m_bytesUsed, SEEK_SET);
    return size;
}

int H264File::readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame)
{
    if(m_file == NULL)
    {
        return -1;
    }

    int bytesRead = fread(m_buf, 1, m_bufSize, m_file);
    if(bytesRead == 0)
    {
        fseek(m_file, 0, SEEK_SET); 
        m_count = 0;
        m_bytesUsed = 0;
        bytesRead = fread(m_buf, 1, m_bufSize, m_file);
        if(bytesRead == 0)        
        {            
            this->close();
            return -1;
        }
    }

    bool bFindStart = false, bFindEnd = false;

    int i = 0, startCode = 3;
    *bEndOfFrame = false;
    for (i=0; i<bytesRead-5; i++)
    {
        if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 1)
        {
            startCode = 3;
        }
        else if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 0 && m_buf[i+3] == 1)
        {
            startCode = 4;
        }
        else 
        {
            continue;
        }
        
        if (((m_buf[i+startCode]&0x1F) == 0x5 || (m_buf[i+startCode]&0x1F) == 0x1) &&
             ((m_buf[i+startCode+1]&0x80) == 0x80))				 
        {
            bFindStart = true;
            i += 4;
            break;
        }
    }

    for (; i<bytesRead-5; i++)
    {
        if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 1)
        {
            startCode = 3;
        }
        else if(m_buf[i] == 0 && m_buf[i+1] == 0 && m_buf[i+2] == 0 && m_buf[i+3] == 1)
        {
            startCode = 4;
        }
        else 
        {
            continue;
        }
        
        if (((m_buf[i+startCode]&0x1F) == 0x7) || ((m_buf[i+startCode]&0x1F) == 0x8) 
            || ((m_buf[i+startCode]&0x1F) == 0x6)|| (((m_buf[i+startCode]&0x1F) == 0x5 
            || (m_buf[i+startCode]&0x1F) == 0x1) &&((m_buf[i+startCode+1]&0x80) == 0x80)))
        {
            bFindEnd = true;
            break;
        }
    }

    bool flag = false;
    if(bFindStart && !bFindEnd && m_count>0)
    {        
        flag = bFindEnd = true;
        i = bytesRead;
        *bEndOfFrame = true;
    }

    if(!bFindStart || !bFindEnd)
    {
        this->close();
        return -1;
    }

    int size = (i<=inBufSize ? i : inBufSize);
    memcpy(inBuf, m_buf, size); 

    if(!flag)
    {
        m_count += 1;
        m_bytesUsed += i;
    }
    else
    {
        m_count = 0;
        m_bytesUsed = 0;
    }

    fseek(m_file, m_bytesUsed, SEEK_SET);
    return size;
}

int getADTSframe(uint8_t * buffer, int buf_size, uint8_t * data ,int* data_size){
    int size = 0;

    if(!buffer || !data || !data_size ){
        return -1;
    }

    while(1){
        if(buf_size  < 7 ){
            return -1;
        }
        //Sync words
        if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0) ){
            size |= ((buffer[3] & 0x03) <<11);     //high 2 bit
            size |= buffer[4]<<3;                //middle 8 bit
        size |= ((buffer[5] & 0xe0)>>5);        //low 3bit
        break;
        }
        --buf_size;
        ++buffer;
    }

    if(buf_size < size){
        return 1;
    }

    memcpy(data, buffer, size);
    *data_size = size;

    return 0;
}

bool AacFile::open(const char *path)
{
    m_file = fopen(path, "rb");
    if(m_file == NULL)
    {      
       return false;
    }
    
    fseek(m_file, 0, SEEK_END);
    m_length = ftell(m_file);
    fseek(m_file, 0, SEEK_SET);

    m_filePointer = (uint8_t *)malloc(m_length);
    if (!m_filePointer) {
        return false;
    }

    if (fread(m_filePointer, 1, m_length, m_file) <= 0) {
        free(m_filePointer);
        m_filePointer = nullptr;
        return false;
    }
    return true;
}

void AacFile::close()
{
    if(m_file)
    {
        fclose(m_file);
        m_file = NULL;
        m_count = 0;
        m_bytesUsed = 0;
    }
    
    if (m_filePointer) {
        free(m_filePointer);
        m_filePointer = nullptr;
    }
    return;
}

int AacFile::readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame)
{
    *bEndOfFrame = false;
  
    int gotsize = 0;
    if (0 != getADTSframe((m_filePointer + m_fileOffset), (m_length - m_fileOffset), (uint8_t *)inBuf, &gotsize)) {
        m_fileOffset = 0;
        return 0;
    }
    m_fileOffset += gotsize;
    if ((m_fileOffset + 100) > m_length) {
        m_fileOffset = 0;
    }
    //printf("%x, %x, %x, %x, %x, %x, %x \n", 
    //    inBuf[0], inBuf[1], inBuf[2], inBuf[3], inBuf[4], inBuf[5], inBuf[6]);
    return gotsize;
}

bool G711File::open(const char *path)
{
    m_file = fopen(path, "rb");
    if(m_file == NULL)
    {      
       return false;
    }
    
    fseek(m_file, 0, SEEK_END);
    m_length = ftell(m_file);
    fseek(m_file, 0, SEEK_SET);

    m_filePointer = (uint8_t *)malloc(m_length);
    if (!m_filePointer) {
        return false;
    }

    if (fread(m_filePointer, 1, m_length, m_file) <= 0) {
        free(m_filePointer);
        m_filePointer = nullptr;
        return false;
    }
    return true;
}

void G711File::close()
{
    if(m_file)
    {
        fclose(m_file);
        m_file = NULL;
        m_count = 0;
        m_bytesUsed = 0;
    }
    
    if (m_filePointer) {
        free(m_filePointer);
        m_filePointer = nullptr;
    }
    return;
}

#define G711_AUDIO_SLICE_LENGTH (160)

int G711File::readFrame(char *inBuf, int inBufSize, bool *bEndOfFrame)
{
    *bEndOfFrame = false;

    if ((m_fileOffset + G711_AUDIO_SLICE_LENGTH ) > m_length) {
        m_fileOffset = 0;
    }
    m_fileOffset += G711_AUDIO_SLICE_LENGTH;
    memcpy(inBuf, (m_filePointer + m_fileOffset), G711_AUDIO_SLICE_LENGTH);

    return (G711_AUDIO_SLICE_LENGTH);
}

