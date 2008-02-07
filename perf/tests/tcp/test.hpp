#define TEST_MSG_SIZE_START 1
#define TEST_MSG_SIZE_STEPS 10
//#define TEST_MSG_COUNT_THRPUT 73350
#define TEST_TIME 5000

// MT test
#define TEST_THREADS 1

// slope for messages smaller than SYS_BREAK
// 100 000 msgs
#define SYS_SLOPE 0.55
#define SYS_OFF 110

#define SYS_BREAK 1024

// slope for messages bigger than SYS_BREAK
// 100 000
#define SYS_SLOPE_BIG 0.89
#define SYS_OFF_BIG -80

//#define TEST_MSG_COUNT_THRPUT_DIVIDER 2.00
//#define TEST_MSG_COUNT_LATENCY 100
#define PORT_NUMBER 5555

struct worker_args_t
{
    int id;
    int msg_size;
    int msg_count;
};
