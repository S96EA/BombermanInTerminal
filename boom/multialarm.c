//
// Created by neild47 on 17-12-14.
//

#include "multialarm.h"

int init_multialarm() {
    head = malloc(sizeof(TimeEvent));
    head->next = head;
    head->prev = head;
    signal(SIGALRM, multialarm_handler);
    return 0;
}


int add_alarm(struct itimerval value, time_handler_t handler) {
    sigset_t blkset;
    sigset_t oldset;
    sigemptyset(&blkset);
    sigaddset(&blkset, SIGALRM);
    sigprocmask(SIG_BLOCK, &blkset, &oldset);

    TimeEvent *newEvent = malloc(sizeof(TimeEvent));
    newEvent->value = value;
    newEvent->handler = handler;
    insertEvent(newEvent);

    sigprocmask(SIG_SETMASK, &oldset, NULL);
    return 0;
}

int insertEvent(TimeEvent *newEvent) {
    if (head->next == head) { //empty
        head->next = newEvent;
        head->prev = newEvent;
        newEvent->prev = newEvent->next = head;
        ignore_interval_setitimer(&newEvent->value);
        return 0;
    }

    //update current time event
    TimeEvent *curr_event = head->next;
    struct itimerval tmp_timer;
    getitimer(ITIMER_REAL, &tmp_timer);
    curr_event->value.it_value = tmp_timer.it_value;

    timersub(&newEvent->value.it_value, &curr_event->value.it_value, &tmp_timer.it_value); //reuse tmp_timer as result

    //newtime < currtime
    if (timerlz(&tmp_timer.it_value)) {
        head->next = newEvent;
        curr_event->prev = newEvent;
        newEvent->next = curr_event;
        newEvent->prev = head;
        timersub(&curr_event->value.it_value, &newEvent->value.it_value, &curr_event->value.it_value);
        ignore_interval_setitimer(&newEvent->value);
        return 0;
    }

    curr_event = curr_event->next;
    while (curr_event != head && timergez(&tmp_timer.it_value)) {
        timersub(&tmp_timer.it_value, &curr_event->value.it_value, &tmp_timer.it_value);
        curr_event = curr_event->next;
    }
    if (timerlz(&tmp_timer.it_value)) {
        curr_event = curr_event->prev;
        timeradd(&tmp_timer.it_value, &curr_event->value.it_value, &tmp_timer.it_value);
    }
    newEvent->value.it_value = tmp_timer.it_value;
    newEvent->prev = curr_event->prev;
    newEvent->next = curr_event;
    curr_event->prev->next = newEvent;
    curr_event->prev = newEvent;
    return 0;
}

int ignore_interval_setitimer(struct itimerval *val) {
    struct itimerval v;
    v.it_value = val->it_value;
    memset(&v.it_interval, 0, sizeof(struct timeval));
    setitimer(ITIMER_REAL, &v, NULL);
    return 0;
}

static void multialarm_handler(int v) {
    TimeEvent *tmp = head->next;
    //delete from event list
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;
    if (head->next != head) {
        ignore_interval_setitimer(&head->next->value);
    }
    tmp->handler(v);//process time event
    //having interval timer
    if (timergz(&tmp->value.it_interval)) {
        tmp->value.it_value = tmp->value.it_interval;
        insertEvent(tmp);
    } else {
        free(tmp);
    }
}
