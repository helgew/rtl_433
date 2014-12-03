#include <stdlib.h>
#include <stdio.h>

#include "timer.h"

diff_timer_t* diff_timer_create() {
    diff_timer_t *timer;

    timer = malloc ( sizeof (diff_timer_t) );

    return timer;
} 

void diff_timer_destroy( diff_timer_t *timerp ) {

    if ( timerp )
    {
        free ( (void*)timerp );
    }
}

int diff_timer_start ( diff_timer_t *timerp ) {
    int rv;

    rv = gettimeofday( &((*timerp).time_start), NULL );

    return rv;
}

int diff_timer_stop ( diff_timer_t *timerp ) {
    int rv;

    rv = gettimeofday( &((*timerp).time_stop), NULL );

    return rv;
}

int diff_timer_show ( diff_timer_t *timerp, const char *descr ) {

    long secs;
    long usecs;

    secs = timerp->time_stop.tv_sec - timerp->time_start.tv_sec;
    usecs = timerp->time_stop.tv_usec - timerp->time_start.tv_usec;
    if ( usecs < 0 )
    {
        secs = secs - 1;
        usecs = 1000000000L - usecs;
    }

    fprintf( stderr, "%s: elapsed: %ld.%09ld secs\n", descr, secs, usecs );

    return 0;
}
