#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include "multialarm.h"

#define PLAY_CHAR '@'
#define BOOM_CHAR 'B'
#define FIRE_CHAR 'F'
#define FPS 30

typedef struct Player {
    int position_x, position_y;
} Player;

typedef struct Boom {
    int position_x, position_y;
    struct Boom *next;
} Boom;


Player *player;
Boom *boom_head;
Boom *boom_tail;

Boom *fire_head;
Boom *fire_tail;

struct itimerval boom_timer;

void add_fire(Boom *fire);

void dead() ;

void time_handler(int v) {
    clear();
    move(player->position_x, player->position_y);
    addch(PLAY_CHAR);

    Boom *b = boom_head->next;
    while (b != NULL) {
        move(b->position_x, b->position_y);
        addch(BOOM_CHAR);
        b = b->next;
    }

    b = fire_head->next;
    int isDead = 0;
    attr_on(COLOR_PAIR(1), NULL);
    while (b != NULL) {
        for (int i = b->position_x - 3; i <= b->position_x + 3; ++i) {
            if (player->position_x == i && player->position_y == b->position_y) {
                isDead = 1;
            }
            move(i, b->position_y);
            addch(FIRE_CHAR);
        }
        for (int j = b->position_y - 3; j <= b->position_y + 3; ++j) {
            if (player->position_x == b->position_x && player->position_y == j) {
                isDead = 1;
            }
            move(b->position_x, j);
            addch(FIRE_CHAR);
        }
        b = b->next;
    }
    attr_off(COLOR_PAIR(1), NULL);

    refresh();
    if (isDead) {
        dead();
    }
}

void dead() {
    exit(0);
}


void boom_handler(int i) {
    Boom *b = boom_head->next;
    boom_head->next = b->next;
    if (b->next == NULL) {
        boom_tail = boom_head;
    }

    add_fire(b);
}

void fire_handler(int i) {
    Boom *f = fire_head->next;
    fire_head->next = f->next;
    if (f->next == NULL) {
        fire_tail = fire_head;
    }

    free(f);
}

void init_win() {
    initscr();
    curs_set(0);
    crmode();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
}

void init_player() {
    player = malloc(sizeof(Player));
    player->position_x = LINES / 2;
    player->position_y = COLS / 2;

    boom_head = malloc(sizeof(Boom));
    boom_head->next = NULL;
    boom_tail = boom_head;

    boom_timer.it_value.tv_sec = 2;

    fire_head = malloc(sizeof(Boom));
    fire_head->next = NULL;
    fire_tail = fire_head;
}

void init_timer() {
    init_multialarm();
    struct itimerval v;
    unsigned interval = 1000000 / FPS;
    v.it_value.tv_usec = interval;
    v.it_value.tv_sec = 0;
    v.it_interval.tv_usec = interval;
    v.it_interval.tv_sec = 0;
    add_alarm(v, time_handler);
}

void add_fire(Boom *fire) {
    fire->next = NULL;

    fire_tail->next = fire;
    fire_tail = fire;

    add_alarm(boom_timer, fire_handler);
}

void add_boom() {
    Boom *new_boom = malloc(sizeof(Boom));
    new_boom->position_x = player->position_x;
    new_boom->position_y = player->position_y;
    new_boom->next = NULL;

    boom_tail->next = new_boom;
    boom_tail = new_boom;

    add_alarm(boom_timer, boom_handler);
}

int main() {
    init_win();
    init_player();
    init_timer();

    while (1) {
        switch (getch()) {
            case KEY_UP:
                player->position_x--;
                break;
            case KEY_DOWN:
                player->position_x++;
                break;
            case KEY_LEFT:
                player->position_y--;
                break;
            case KEY_RIGHT:
                player->position_y++;
                break;
            case 'c':
                add_boom();
                break;
            default:
                goto out;
        }
    }
    out:
    endwin();
}