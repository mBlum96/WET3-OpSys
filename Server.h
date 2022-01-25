#ifndef _SERVER_H
#define _SERVER_H

//includes
#include "Headers.h"

//structs

typedef struct WRQ{
    uint16_t OPcode;
    char fileName [MAX_STRING+1];
    char transMode [MAX_WRQ_SIZE];
}__attribute__((packed)) WRQ;

typedef struct ACK{
    uint16_t OPcode;
    uint16_t blockNum;
}__attribute__((packed)) ACK;

typedef struct DATA{
    uint16_t OPcode;
    uint16_t blockNum;
    char data [MAX_WRQ];
}__attribute__((packed)) DATA;

typedef struct ERROR{
    uint16_t OPcode;
    uint16_t errorCode;
    char errMsg[MAX_ERR_LEN + 1];
}__attribute__((packed)) ERROR;

void handleData();
void recieve_WRQ();
void send_ACK();



#endif