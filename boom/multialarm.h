//
// Created by neild47 on 17-12-14.
//

#ifndef TER_MULTIALARM_H
#define TER_MULTIALARM_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>

#define timergez(t) ((t)->tv_sec > 0 || ((t)->tv_sec == 0 && (t)->tv_usec >= 0))
#define timergz(t) ((t)->tv_sec > 0 || ((t)->tv_sec == 0 && (t)->tv_usec > 0))
#define timerlez(t) ((t)->tv_sec < 0 || ((t)->tv_sec == 0 && (t)->tv_usec <= 0))
#define timerlz(t) ((t)->tv_sec < 0 || ((t)->tv_sec == 0 && (t)->tv_usec < 0))
#define timernz(t) ((t)->tv_sec != 0 || (t)->tv_usec != 0)

typedef void (*time_handler_t)(int);

typedef struct TimeEvent {
    struct itimerval value;
    struct TimeEvent *prev, *next;
    time_handler_t handler;
} TimeEvent;

static TimeEvent *head;

static int insertEvent(TimeEvent *newEvent);

static int ignore_interval_setitimer(struct itimerval *val);

int init_multialarm();

static void multialarm_handler(int v);

int add_alarm(struct itimerval value, time_handler_t handler);


#endif //TER_MULTIALARM_H
