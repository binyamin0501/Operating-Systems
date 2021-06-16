#include "ttftps.h"

int main(int argc, char* argv[]) {

	if (argc < 2) {
		fprintf(stderr, "TTFTP_ERROR: invalid amount of arguments\n");
		exit(1);
	}

	int sock;
	unsigned short port_num;
    unsigned int client_add_len;
	uint16_t block_num;
	FILE* file_desc;
	char Buffer[BUFFER_MAX_SIZE] = {0};
	port_num = atoi(argv[1]);
	Sock_Addr_In tftp_add = {0}, user_add = {0};
	WRQ_Msg request_msg;
	
	if ((sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP)) < 0)
		perror("TTFTP_ERROR: create socket failure\n");

	memset(&tftp_add, 0, sizeof(tftp_add));
    set_server_add(port_num, &tftp_add);
	// tftp_add.sin_family = AF_INET;
	// tftp_add.sin_addr.s_addr = htonl(INADDR_ANY);
	// tftp_add.sin_port = htons(port_num);

	if (bind(sock, (PSock_addr) & tftp_add, (socklen_t)sizeof(tftp_add)) < 0) {
        close(sock);
		perror("TTFTP_ERROR: bind failure\n");
	}
	
	while (true) {
		client_add_len = sizeof(user_add);
		if (recvfrom(sock, Buffer, BUFFER_MAX_SIZE, 0, (PSock_addr) & user_add, &client_add_len) < 0) {
			perror("TTFTP_ERROR: recvfrom failure\n");
			continue; 
		}
		if (Convert_Buffer_to_WRQ(&request_msg, Buffer)) {
			cerr << "FLOWERROR: wrong opcode" << endl;
			continue; 
		}

		file_desc = fopen(request_msg.file_name, "w");
		if (file_desc == NULL) {
			perror("TTFTP_ERROR:");
			continue; 
		}

		cout << "IN:WRQ," << request_msg.file_name << "," << request_msg.trans_mode << endl;
		block_num = 0;
		if (ACK_Message(block_num, sock, user_add)) {
			continue; 
		}
		if (Request_Routine(sock, user_add, client_add_len, file_desc)) {
			cout << "RECVFAIL" << endl;
			if (fclose(file_desc))
			{
				perror("TTFTP_ERROR:");
				exit(1);
			}
			continue;
		}
	}
	close(sock);
	return 0;
}

void set_server_add(int port_num, PSock_Addr_In tftp_add)
{
	tftp_add->sin_family = AF_INET;
	tftp_add->sin_addr.s_addr = htonl(INADDR_ANY);
	tftp_add->sin_port = htons(port_num);
}

void Buffer_Init(char* buffer, int size){
	for (int i = 0; i < size; i++)
		buffer[i] = 0;
}

int Request_Routine(int sock_number, Sock_Addr_In Client_add, unsigned int client_add_len, FILE* file_desc) {

	int timeoutExpiredCount = 0, recvMsgSize,ready, block_num = 0, lastWriteSize;
	fd_set rfds;
	char echoBuffer[BUFFER_MAX_SIZE] = {0};
	Data_Msg  data_messege;
	struct timeval tv;

	tv.tv_sec = WAIT_FOR_PACKET_TIMEOUT;

	do
	{
		recvMsgSize = -1;
		do
		{
			do
			{
				FD_ZERO(&rfds);
				FD_SET(sock_number, &rfds);
				ready = select(sock_number + 1, &rfds, NULL, NULL, &tv);
				if (ready > 0) { 
					Buffer_Init(echoBuffer, BUFFER_MAX_SIZE);
					recvMsgSize = recvfrom(sock_number, &data_messege, BUFFER_MAX_SIZE, 0, (PSock_addr) & Client_add, &client_add_len);
					if (recvMsgSize < 0) {
						perror("TTFTP_ERROR: recvfrom failure\n");
					}
				}
				else if (ready == 0)
				{
					cerr << "FLOWERROR: timeout expired" << endl;
					if (ACK_Message(block_num, sock_number, Client_add)) {
						return 1;
					}
					timeoutExpiredCount++;
					if (timeoutExpiredCount >= NUMBER_OF_FAILURES)
					{
						cerr << "FLOWERROR: number of failures exceeded" << endl;
						return 1;
					}
				}
				else {
					perror("TTFTP_ERROR:");
				}
			} while (ready >= 0 && recvMsgSize < 0);

			if (ntohs(data_messege.opcode) != DATA) {
				cerr << "opcode recieved not valid" << endl;
				return  1;
			}

			if (block_num + 1 != ntohs(data_messege.block_num)) {
				cerr << "FLOWERROR: wrong block number" << endl;
				return 1;
			}
		} while (false);

		timeoutExpiredCount = 0;
		block_num++;
		lastWriteSize = fwrite(data_messege.data, sizeof(char), recvMsgSize - HEADER_SIZE, file_desc);
		if (lastWriteSize != (recvMsgSize - HEADER_SIZE)) {
			perror("TTFTP_ERROR:");
			return 1;
		}
		cout << "WRITING: " << (recvMsgSize - HEADER_SIZE) << endl;
		if (ACK_Message(block_num, sock_number, Client_add)) {
			return 1;
		}
	} while (recvMsgSize == BUFFER_MAX_SIZE); 
	cout << "RECVOK" << endl;
	fclose(file_desc);
	return 0;
}


int ACK_Message(uint16_t block_num, int sock_fd, Sock_Addr_In Client_add) {
	ACK_Msg ack_messege;
	ack_messege.block_num = htons(block_num);
	ack_messege.opcode = htons(ACK);
	if (sendto(sock_fd, &ack_messege, HEADER_SIZE, 0, (PSock_addr) & Client_add, sizeof(Client_add)) != HEADER_SIZE) {
		perror("TTFTP_ERROR:");
		return 1;
	}
	else {
		cout << "OUT:ACK," << block_num << endl;
		return 0;
	}
}

int Convert_Buffer_to_WRQ(PWRQ_Msg msg, char* buffer) {
	memcpy(&(msg->opcode), buffer, OPCODE_SIZE);
	msg->opcode = ntohs(msg->opcode);
	if (msg->opcode != WRQ) {
		return 1;
	}
	int i = 0;
	while (buffer[i + OPCODE_SIZE] != 0) {
		msg->file_name[i] = buffer[i + OPCODE_SIZE];
		i++;
	}
	msg->file_name[i] = '\0';
	int j = 0;
	i++;
	while (buffer[i + OPCODE_SIZE] != 0) {
		msg->trans_mode[j] = buffer[i + OPCODE_SIZE];
		i++;
		j++;
	}
	msg->trans_mode[j] = '\0';
	return 0;
}