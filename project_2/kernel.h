/*
 * Kernel.h
 *
 * Created: 3/10/2017 11:00:55 AM
 *  Author: Josh
 */

#ifndef _KERNEL_H
#define _KERNEL_H

#include "os.h"
#include "error_codes.h"
 
/*
 *    Definitions.
 */
#define Disable_Interrupt()   asm volatile ("cli"::)
#define Enable_Interrupt()    asm volatile ("sei"::)

/*
 *    Type Definitions.
 */
typedef void (*voidfuncptr) (void);    // Pointer to void f(void).

typedef enum process_states
{
    DEAD = 0,
    BLOCKED,
    READY,
    RUNNING,
    WAITING
} PROCESS_STATES;

typedef enum kernel_request_type
{
    NONE = 0,
    ABORT,
    CREATESY,
    CREATEPD,
    CREATERR,
    NEXT,
    TERMINATE,
    CHANINIT,
    SEND,
    RECV,
    WRITE
} KERNEL_REQUEST_TYPE;

typedef enum process_priorities
{
    SYSTEM,
    PERIODIC,
    ROUNDROBIN
} PROCESS_PRIORITIES;

/*
 *    Structures.
 */
typedef struct process_descriptor
{
    unsigned char*      sp;        // Stack pointer into the "workSpace".
    unsigned char       workSpace[WORKSPACE];
    PROCESS_STATES      state;
    voidfuncptr         code;    // Function to be executed as a task.
    KERNEL_REQUEST_TYPE request;
    int                 creation_arg;
    int                 pid;
    unsigned int        return_value;
    ERROR_CODE          error;
    PROCESS_PRIORITIES  priority;
    TICK                period;
    TICK                wcet;
    TICK                offset;
    unsigned int        num_ticks_remaining;
    CHAN                channel;
    int                 message;
} PD;

typedef struct channel_descriptor
{
    CHAN         number;
    PD*          sender;
    unsigned int num_receivers;
    PD*          receivers[MAXTHREAD];
} CHAND;

#endif /* _KERNEL_H */