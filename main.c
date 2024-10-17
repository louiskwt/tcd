#include <stdio.h>
#include <ncurses.h>
#include <time.h>

void printing();

int main (void)
{
    // initial set up
    clock_t begin = clock();
    int count = 60;
    double time_spent = 0;
    double prev_time = 0;

    // start ncurses window
    initscr();
    refresh();
    curs_set(0); // remove cursor

    // count down logic
    do
    {
        mvprintw(10, 10, "00 : 00 : %02i", count);
        refresh();
        time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
        if (time_spent - prev_time >= 1)
        {
            count--;
            prev_time++;
        }
    }
    while(time_spent <= 60);

    // wait for key press and close window
    mvprintw(10, 10, "\npress any key to exit...");
    refresh();
    getch();
    endwin();


    return 0;
}