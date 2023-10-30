#pragma once
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> //pid_t
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#define abc_signal_helper(n)     SIG##n
#define abc_signal_value(n)      abc_signal_helper(n)
#define abc_str_value_helper(n)   #n
#define abc_str_value(n)          abc_str_value_helper(n)
#define ABC_SHUTDOWN_SIGNAL      QUIT
#define ABC_TERMINATE_SIGNAL     TERM
typedef struct {
    int     signo;
    char   *signame;
    char   *name;
    void  (*handler)(int signo, siginfo_t *siginfo, void *ucontext);
}abc_signal_t;

#define ABC_PRIV_SZ 64
#define ABC_MAX_PROCESSES 64
#define ABC_INVALID_PID  -1
#define abc_errno                  errno
#define abc_getpid   getpid
#define abc_getppid  getppid

typedef pid_t abc_pid_t;
typedef struct{
    abc_pid_t pid;
    char priv[ABC_PRIV_SZ];
}abc_process_t;



