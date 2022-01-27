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
#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <stdint.h>

using std::string;
using std::ofstream;
using std::ifstream;
using std::map;
using std::unordered_map;

//enums:
enum Sizes{
    MAX_WRQ = 512,
    MAX_STRING = 254,
    MAX_DATA = 512,
    MAX_PACKET = 516,
    MAX_WRQ_SIZE = 1500,
    MAX_ERR_LEN = 40,
    OPCODE_BYTES = 2
};

enum Errors{
    UNKNOWN_USER = 7,
    ILLEGAL_TFTP_OPERATION = 4,
    UNEXPECTED_PACKET = 4,
    ILLEGAL_WRQ = 4,
    FILE_ALREADY_EXISTS = 6,
    BAD_BLOCKING_NUMBER = 0,
    ABANDONING_FILE_TRANSMISSION = 0
};

enum OPcode{
    ACK_OP_CODE = 4,
    WRQ_ACK_BLOCK_NUM = 0,
    WRQ_OPCODE = 2,
    DATA_OPCODE = 3,
    ERROR_OPCODE = 5
};

enum Times{
    SELECT_TIMEOUT = 1
};