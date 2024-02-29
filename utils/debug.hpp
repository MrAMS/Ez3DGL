#pragma once

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE


#define log_with_info(FMT, ...) \
    do{ \
        printf("%s:%d %s:" "\n",    \
            __FILE__, __LINE__, __func__);  \
        printf(ANSI_FMT("[LOG] " FMT, ANSI_FG_BLUE) "\n", ## __VA_ARGS__); \
    }while(0)

#define assert_with_info(COND, FMT, ...) \
    if(!(COND)){ \
        printf("%s:%d %s:" "\n",    \
            __FILE__, __LINE__, __func__);  \
        printf(ANSI_FMT("[ASSERT] " FMT, ANSI_FG_RED) "\n", ## __VA_ARGS__); \
    } \
    assert(COND)

#define panic_with_info(FMT, ...) \
    assert_with_info(0, FMT, ## __VA_ARGS__)
