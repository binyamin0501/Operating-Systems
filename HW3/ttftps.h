#ifndef _TTFTPS_H_
#define _TTFTPS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

#define BUFFER_MAX_SIZE 516
#define DATA_MAX_SIZE 512
#define OPCODE_SIZE 2
#define HEADER_SIZE 4
#define NUMBER_OF_FAILURES 7
#define WAIT_FOR_PACKET_TIMEOUT 3;
#define ERROR -1

enum OP_CODE {WRQ = 2, DATA = 3, ACK = 4};

typedef struct sockaddr_in Sock_Addr_In, * PSock_Addr_In;
typedef struct _Data_Msg Data_Msg, * PData_Msg;
typedef struct _WRQ_Msg WRQ_Msg, * PWRQ_Msg;
typedef struct _ACK_Msg ACK_Msg, * PACK_Msg;
typedef struct sockaddr * PSock_addr;

struct _Data_Msg {
	uint16_t opcode;
	uint16_t block_num;
	char data[BUFFER_MAX_SIZE - HEADER_SIZE];
}__attribute__((packed));

struct _WRQ_Msg {
	uint16_t opcode;
	char file_name[BUFFER_MAX_SIZE - OPCODE_SIZE];
	char trans_mode[BUFFER_MAX_SIZE - OPCODE_SIZE];
}__attribute__((packed));

struct _ACK_Msg {
	uint16_t opcode;
	uint16_t block_num;
} __attribute__((packed));

void Buffer_Init (char* buffer, int size);
int ACK_Message (uint16_t block_num, int sock_fd, Sock_Addr_In Client_add);
int Request_Routine (int sock_number, Sock_Addr_In echoClntAddr, unsigned int cliAddrLen, FILE* file_desc);
void set_server_add(int port_num, PSock_Addr_In tftp_add);
int Convert_Buffer_to_WRQ (PWRQ_Msg msg, char* buffer);

#endif // _TTFTPS_H_