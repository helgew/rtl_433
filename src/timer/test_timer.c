#include <unistd.h>
#include <time.h>
#include "timer.h"

int main()
{
    int i;

    diff_timer_t *timer1;
    timer1 = diff_timer_create();
    for( i =0; i < 10; i++ )
    {
        diff_timer_start(timer1);
        sleep(1);
	diff_timer_stop(timer1);
        diff_timer_show(timer1, "t1");
    }
    return 0;
}
