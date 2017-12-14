//
// Created by neild47 on 17-12-14.
//

#include "multialarm.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>

struct itimerval val;

static void handler1(int v) {
    printf("handler1...\n");
}

static void handler2(int v) {
    printf("handler2...\n");
}

static void handler3(int v) {
    printf("handler3...\n");
}

static void handler4(int v) {
    printf("handler4...\n");
}

static void handler5(int v) {
    printf("handler5...\n");
}

static int testmultialarm() {
    init_multialarm();
    memset(&val, 0, sizeof(struct itimerval));
    val.it_value.tv_sec = 1;
    val.it_interval.tv_usec = 1000000 / 10;
    add_alarm(val, handler1);
    memset(&val, 0, sizeof(struct itimerval));
    val.it_value.tv_sec = 1;
    val.it_interval.tv_sec = 1;
    add_alarm(val, handler2);
    while (1) {
        sleep(1);
    }
}
