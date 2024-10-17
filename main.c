#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int convert_hour(int minute);
int convert_minute(int second);
int convert_second(int second);

int main (int argc, char* argv[])
{
    // check command line argument
    if (argc < 2 || argc > 2)
    {
        printf("Usage: ./tcd MINUTES\n");
        return 1;
    }
    
    // initial set up
    int total_second = atoi(argv[1]) * 60;
    int minute = convert_minute(total_second);
    int hour = convert_hour(minute);
    int second = convert_second(total_second);

    double time_spent = 0;
    double prev_time = 0;

    // start ncurses window
    initscr();
    refresh();
    curs_set(0); // remove cursor

    clock_t begin = clock();
    // count down logic
    do
    {
        mvprintw(10, 10, "%02i : %02i : %02i", hour, minute, second);
        refresh();
        time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC; // in second
        
        if (time_spent - prev_time >= 1)
        {
            total_second--;
            minute = convert_minute(total_second);
            hour = convert_hour(minute);
            second = convert_second(total_second);
            prev_time++;
        }
    }
    while(total_second >= 0);

    // wait for key press and close window
    mvprintw(10, 10, "\npress any key to exit...");
    refresh();
    getch();
    endwin();
    printf("Time spent: %f, total second: %d", time_spent, total_second);
    return 0;
}

int convert_hour(int minute)
{ 
    if (minute < 60)
    {
        return 0;
    } else 
    {
        return round(minute / 60);
    }
}

int convert_minute(int second)
{
    if (second <= 60)
    {
        return 0;
    }
    else
    {
        return round(second / 60);
    }
}

int convert_second(int second)
{
   return second % 60;
}
