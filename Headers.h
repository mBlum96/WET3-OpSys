//includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

//enums:
typedef enum Sizes{
    MAX_WRQ = 512,
    MAX_STRING = 254,
    MAX_DATA = 512,
    MAX_DATA_PACKET = 516,
    MAX_WRQ_SIZE = 1500,
    MAX_ERR_LEN = 40,
    OPCODE_BYTES = 2
};

typedef enum Errors{
    UNKNOWN_USER = 7,
    ILLEGAL_TFTP_OPERATION = 4,
    UNEXPECTED_PACKET = 4,
    ILLEGAL_WRQ = 4,
    FILE_ALREADY_EXISTS = 6,
    BAD_BLOCKING_NUMBER = 0,
    ABANDONING_FILE_TRANSMISSION = 0
};
