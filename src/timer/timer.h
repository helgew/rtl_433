#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>


typedef struct {
    struct timeval time_start;
    struct timeval time_stop;
} diff_timer_t;

diff_timer_t* diff_timer_create();
void diff_timer_destroy( diff_timer_t *timerp );
int diff_timer_start ( diff_timer_t *timerp );
int diff_timer_stop ( diff_timer_t *timerp );
int diff_timer_show ( diff_timer_t *timerp, const char *descr );

#endif
