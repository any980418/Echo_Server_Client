#pragma once

#define BUF_SIZE 512

// Define Command
enum CMD
{
    CMD_SEND_MSG = 0x01,
    CMD_SAVE_MSG,
    CMD_DELETE_MSG,
    CMD_MSG_LIST,

    RES_SEND_MSG,
    RES_SAVE_MSG,
    RES_DELETE_MSG,
    RES_MSG_LIST,
};

struct PacketHeader
{
    char            header[5]       = "AA11";
    unsigned int    size            = 0;
};

struct PacketBody
{
    unsigned int    cmd             = 0x00;
    unsigned int    dataLen         = 0;
    char            data[BUF_SIZE]  = {};
};

struct PacketTail
{
    char            tail[5]         = "11AA";
};

struct Packet
{
    PacketHeader    pHead;
    PacketBody      body;
    PacketTail      pTail;
};