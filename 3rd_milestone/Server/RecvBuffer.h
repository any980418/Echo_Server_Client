#pragma once

#include <vector>
using namespace std;

#define OUT

struct Packet;

/*--------------
	RecvBuffer
----------------*/

class RecvBuffer
{
	enum { BUFFER_COUNT = 100 };

public:
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	void    Clean();
	bool    OnRead(int numOfBytes);
	bool    OnWrite(int numOfBytes);

	char*	ReadPos() { return &_buffer[_readPos]; }
	char*   WritePos() { return &_buffer[_writePos]; }
	int   	DataSize() { return _writePos - _readPos; }
	int   	FreeSize() { return _capacity - _writePos; }

	int		PopPacket(OUT Packet* packet);

private:
	int     		_capacity = 0;
	int     		_bufferSize = 0;
	int     		_readPos = 0;
	int     		_writePos = 0;
	vector<char>	_buffer;
};
