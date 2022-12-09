#include "RecvBuffer.h"
#include "Packet.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

/*--------------
	RecvBuffer
----------------*/

RecvBuffer::RecvBuffer(int bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int dataSize = DataSize();
	if (dataSize == 0)
	{
		// 딱 마침 읽기+쓰기 커서가 동일한 위치라면, 둘 다 리셋.
		_readPos = _writePos = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땅긴다.
		if (FreeSize() < _bufferSize)
		{
			memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}

int RecvBuffer::PopPacket(OUT Packet* packet)
{
	const char header[]	= "AA11";
	const char tail[]	= "11AA";

	if(DataSize() < sizeof(PacketHeader))
		return -1;

	for(int i = _readPos; i < _writePos - sizeof(header); i++)
	{
		if(strncmp(&_buffer[i], header, sizeof(header)) == 0)
		{
			if((_writePos - i) < sizeof(Packet))
				return -1;

			memcpy(packet, &_buffer[i], sizeof(Packet));
			if(strcmp(packet->pTail.tail, tail) == 0)
			{
				return (i - _readPos) + sizeof(Packet);
			}
		}
	}

	return -1;
}