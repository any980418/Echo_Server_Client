#pragma once

#include <iostream>
#include <cstring>
using namespace std;

#include "Packet.h"

#define QUEUE_SIZE 5120

class CircularQueue
{
public:
    bool enqueue(char* data, int len);
    bool dequeue(Packet* packet);

    bool isEnoughBuffer(int size);
    bool isEnoughData(int size);

    int FindHeader(PacketHeader* packetHeader);
    int ExtractData(char* buf, int len, int idx);

private:
    char    _queue[QUEUE_SIZE];
    int     _front  = 0;
    int     _rear   = 0;
    int     _restSize = QUEUE_SIZE;
};