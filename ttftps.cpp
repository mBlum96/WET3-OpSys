#include "Server.hpp"

map<uint32_t,int> clientsMap;
map<uint32_t,char*> fileMap;
map<uint32_t,int> errore_count;
map<uint32_t,int> time_count;
map<uint32_t,sockaddr_in> ip_to_struct;  



int main(int argc, char* argv[]){
    int port = atoi(argv[1]);
    int timeOut_int = atoi(argv[2]);
    int numOfTimeOuts = atoi(argv[3]);
    int udp_fd = socket(AF_INET, SOCK_DGRAM,0);
    struct timeval timeOut;
    timeOut.tv_usec = 0;
    struct sockaddr_in serverAddress = {0};
    struct sockaddr_in clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr= htonl(INADDR_ANY);//hexa to network long
    serverAddress.sin_port=htons(port);//setting the port

    int bound_sock = bind(udp_fd, (struct sockaddr*)&serverAddress,
    sizeof(serverAddress));
    if(bound_sock < 0){
        perror("TTFTP_ERROR");
        exit(1);
    }
    socklen_t sockLen = sizeof(clientAddress);
    char buff[MAX_WRQ_SIZE+1];
    if(udp_fd<0){
        perror("TTFTP_ERROR");
        exit(1);
    }
    // int countErr = 0;
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(udp_fd,&rset);
    
    while(true){
		if(clientsMap.size()==0)
			timeOut.tv_sec = 3;
		else{
			timeOut.tv_sec = timeOut_int-time_count.rbegin()->second;
		}
        int select_num = select(udp_fd+1,&rset,NULL,NULL,&timeOut);
        if(select_num<0){
            perror("TTFTP_ERROR");
            shutdown(udp_fd,SHUT_RDWR);
            exit(1);
        }
        if(select_num==0){
            if(clientsMap.size()==0)
				continue;
			else
			{
				uint32_t ip=time_count.rbegin()->first;
                clientAddress=ip_to_struct.find(ip)->second;
				errore_count.find(ip)->second++;
				if(errore_count.find(ip)->second>numOfTimeOuts)
				{
						 ERROR send_err_msg = 
						{htons(ERROR_OPCODE), htons(0), "Abandoning file transmission"};
						if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
						(struct sockaddr *) &clientAddress,
						sizeof(clientAddress)) <0){
							perror("TTFTP_ERROR");
						}
						shutdown(udp_fd,SHUT_RDWR);
						exit(1);
				}
				for(auto i=time_count.begin();i!=time_count.end();i++)
				{
						i->second+=timeOut_int-time_count.rbegin()->second;
				}
				ACK ack = generate_ACK();
				ack.blockNum=clientsMap.find(ip)->second;
				if(sendto(udp_fd, &ack, MAX_PACKET, 0,
						(struct sockaddr *) &clientAddress,
						sizeof(clientAddress)) <0){
							perror("TTFTP_ERROR");
						}
				time_count.erase(ip);
				time_count.insert(std::pair<uint32_t,int> (ip,0));
            }
        }
        else{
            uint32_t ip=time_count.rbegin()->first;
            clientAddress=ip_to_struct.find(ip)->second;
			for(auto i=time_count.begin();i!=time_count.end();i++)
				{
						i->second+=timeOut_int-time_count.rbegin()->second;
				}
			time_count.erase(ip);
			time_count.insert(std::pair<uint32_t,int> (ip,0));
            // int numBlock = 0;
            int recievedOPcode = recvfrom(udp_fd, buff, MAX_WRQ_SIZE, 0,
            (struct sockaddr *) &clientAddress, &sockLen);
            if(recievedOPcode<0){
                perror("RRFTP_ERROR");
                shutdown(udp_fd,SHUT_RDWR);
                exit(1);
            }
            if(strlen(buff)>MAX_PACKET){
                perror("TTFTP_ERROR");
                shutdown(udp_fd,SHUT_RDWR);
                exit(1);
            }
            else{
                char opCodeChar[2];
                memcpy(opCodeChar,buff,2);
                if(atoi(opCodeChar)==WRQ_OPCODE){
                    recieve_WRQ(buff,udp_fd,clientAddress);
                }
                else if(atoi(opCodeChar)==DATA_OPCODE){
                    recieve_DATA(buff,udp_fd,clientAddress);
                }
                else{
                    ERROR send_err_msg = 
                    {htons(ERROR_OPCODE), htons(4), "Illegal TFTP operation"};
                    if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
                    (struct sockaddr *) &clientAddress,
                    sizeof(clientAddress)) <0){
                        perror("TTFTP_ERROR");
                    }
                }
            }
        }
    }
    return 0;
}

void recieve_DATA(char* dataBuffer, int udp_fd, struct sockaddr_in clientAddress){
    DATA data;
    memcpy(&data, dataBuffer, MAX_PACKET);
    if(clientsMap.find(clientAddress.sin_addr.s_addr)==clientsMap.end()){
        ERROR send_err_msg = 
        {htons(ERROR_OPCODE), htons(7), "Unknown user"};
        if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
        (struct sockaddr *) &clientAddress,
        sizeof(clientAddress)) <0){
            perror("TTFTP_ERROR");
        }
        return;
    }
    int i = clientsMap.find(clientAddress.sin_addr.s_addr)->second;
    if(data.blockNum!= i+1)
    {
        ERROR send_err_msg = 
        {htons(ERROR_OPCODE), htons(0), "Bad block number"};
        if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
        (struct sockaddr *) &clientAddress,
        sizeof(clientAddress)) <0){
            perror("TTFTP_ERROR");
        }
        return;
    }
    clientsMap.find(clientAddress.sin_addr.s_addr)->second++;
    ofstream out;
    out.open(fileMap.find(clientAddress.sin_addr.s_addr)->second, std::ios::app);
    out << data.data;
    if(strlen(data.data)<MAX_DATA){
        fileMap.erase(clientAddress.sin_addr.s_addr);
        clientsMap.erase(clientAddress.sin_addr.s_addr);
		time_count.erase(clientAddress.sin_addr.s_addr);
		errore_count.erase(clientAddress.sin_addr.s_addr);
        ip_to_struct.erase(clientAddress.sin_addr.s_addr);
    }
}

void recieve_WRQ(char* dataBuffer, int udp_fd, struct sockaddr_in clientAddress) {
    WRQ wrq;
	memcpy(&wrq, dataBuffer, MAX_PACKET);
    if(strchr(wrq.fileName,'/')!=NULL){
        ERROR send_err_msg = 
        {htons(ERROR_OPCODE), htons(4), "Illegal WRQ"};
        if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
        (struct sockaddr *) &clientAddress,
        sizeof(clientAddress)) <0){
            perror("TTFTP_ERROR");
        }
        return;
    }
    ifstream f(dataBuffer);
    if(f.good())
    {
        ERROR send_err_msg = 
        {htons(ERROR_OPCODE), htons(6), "File already exists"};
        if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
        (struct sockaddr *) &clientAddress,
        sizeof(clientAddress)) <0){
            perror("TTFTP_ERROR");
        }
        return;
    }
    if(clientsMap.find(clientAddress.sin_addr.s_addr)!=clientsMap.end()){
        ERROR send_err_msg = 
        {htons(ERROR_OPCODE), htons(4), "Unexpected packet"};
        if(sendto(udp_fd, &send_err_msg, MAX_PACKET, 0,
        (struct sockaddr *) &clientAddress,
        sizeof(clientAddress)) <0){
            perror("TTFTP_ERROR");
        }
        return;
    }
    clientsMap.insert(std::pair<uint32_t,int> (clientAddress.sin_addr.s_addr,0));
    fileMap.insert(std::pair<uint32_t,char*>(clientAddress.sin_addr.s_addr, wrq.fileName));
	errore_count.insert(std::pair<uint32_t,int> (clientAddress.sin_addr.s_addr,0));
	time_count.insert(std::pair<uint32_t,int> (clientAddress.sin_addr.s_addr,0));
    ip_to_struct.insert(std::pair<uint32_t,struct sockaddr_in> (clientAddress.sin_addr.s_addr,clientAddress));
    ACK ack = generate_ACK();
    if(sendto(udp_fd, &ack, MAX_PACKET, 0,
        (struct sockaddr *) &clientAddress,
        sizeof(clientAddress)) <0){
        perror("TTFTP_ERROR");
    }    
    // return wrq;
}

ACK generate_ACK(){
    ACK ack;
    ack.OPcode = ACK_OP_CODE;
    ack.blockNum = WRQ_ACK_BLOCK_NUM;
    return ack;
}