#include "Server.h"



int main(int argc, char* argv[]){
    if (argc != 4){
        exit(1);
    }
    

    return 0;
}

void WRQ_parser(char* dataBuffer, WRQ* wrq) {
	memcpy(&wrq->OPcode, dataBuffer, OPCODE_BYTES);
	wrq->OPcode = ntohs(wrq->OPcode);    // fix network byte order
    size_t fileNameLen = strlen(dataBuffer + 2);
    strncpy(wrq->fileName, dataBuffer+OPCODE_BYTES, fileNameLen);
    

}