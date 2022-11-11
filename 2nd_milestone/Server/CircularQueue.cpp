#include "CircularQueue.h"

bool CircularQueue::enqueue(char* data, int len)
{
    if(isEnoughBuffer(len) == false)
        return false;

    if(_rear + len < QUEUE_SIZE)
    {
        memcpy(_queue + _rear, data, len);
        _rear += len;
    }
    else
    {
        int offset = QUEUE_SIZE - _rear;
        memcpy(_queue + _rear, data, offset);
        memcpy(_queue, data + offset, len - offset);
        _rear = len - offset;
    }
    _restSize -= len;

    return true;
}

bool CircularQueue::dequeue(Packet* packet)
{
    //if(isEnoughData(sizeof(Packet)) == false)
    //    return false;

    PacketHeader packetHeader;
    int headerIdx = FindHeader(&packetHeader);
    if(headerIdx == -1)
        return false;

    ExtractData((char*)packet, packetHeader.size, headerIdx);
    
    if(strcmp(packet->pTail.tail, "11AA") == 0)
    {
        _restSize += (((headerIdx - _front) + QUEUE_SIZE) % QUEUE_SIZE);
        _restSize += sizeof(Packet);
        _front = (headerIdx + sizeof(Packet)) % QUEUE_SIZE;

        return true;
    }

    int tailIdx = FindTail();
    if(tailIdx != -1)
    {
        _restSize += (((tailIdx + sizeof(PacketTail) - _front) + QUEUE_SIZE) % QUEUE_SIZE);
        _front = (tailIdx + sizeof(PacketTail)) % QUEUE_SIZE;
    }

    return false; // 수정
}

bool CircularQueue::isEnoughBuffer(int size)
{
    return _restSize - size > 0;
}

bool CircularQueue::isEnoughData(int size)
{
    return QUEUE_SIZE - _restSize >= size;
}

int CircularQueue::FindHeader(PacketHeader* packetHeader)
{
    int len = sizeof(packetHeader->header);
    char buf[len];
    
    int endIdx = (_rear - (len - 1) + QUEUE_SIZE) % QUEUE_SIZE;
    for(int i = _front; i != endIdx; i = (i + 1) % QUEUE_SIZE)
    {
        ExtractData(buf, len, i);
        if(strcmp(buf, packetHeader->header) == 0)
        {
            ExtractData((char*)packetHeader, sizeof(PacketHeader), i);
            return i;
        }
    }

    cout << "Not Exist Header(AA11)" << endl;
    return -1;
}

int CircularQueue::FindTail()
{
    PacketTail packetTail;
    int size = sizeof(PacketTail);
    
    int endIdx = (_rear - (size - 1) + QUEUE_SIZE) % QUEUE_SIZE;
    for(int i = _front; i != endIdx; i = (i + 1) % QUEUE_SIZE)
    {
        ExtractData((char*)&packetTail, size, i);
        if(strcmp(packetTail.tail, "11AA") == 0)
            return i;
    }
    
    cout << "Not Exist Tail(11AA)" << endl;
    return -1;
}

int CircularQueue::ExtractData(char* buf, int len, int idx)
{
    if(idx + len < QUEUE_SIZE)
    {
        memcpy(buf, _queue + idx, len);
        return idx + len;
    }
    else
    {
        int offset = QUEUE_SIZE - idx;
        
        memcpy(buf, _queue + idx, offset);
        memcpy(buf + offset, _queue, len - offset);
        return len - offset;
    }
}