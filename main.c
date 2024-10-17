#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int convert_hour(int minute);
int convert_minute(int minute);
int convert_second(int minute);

int main (int argc, char* argv[])
{
    // check command line argument
    if (argc < 2 || argc > 2)
    {
        printf("Usage: ./tcd MINUTES");
        return 1;
    }

    // initial set up
    clock_t begin = clock();
    int count = atoi(argv[1]);
    int minute = convert_minute(count);
    int hour = convert_hour(count);
    int second = convert_second(count);
    double time_spent = 0;
    double prev_time = 0;

    // start ncurses window
    initscr();
    refresh();
    curs_set(0); // remove cursor

    // count down logic
    do
    {
        mvprintw(10, 10, "%02i : %02i : %02i", hour, minute, second);
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

int convert_hour(int minute)
{
    if (minute < 60)
    {
        return 0;
    } else 
    {
        return (int)(minute / 60);
    }
}

int convert_minute(int minute)
{
    if (minute <= 1)
    {
        return 0;
    }
    else
    {
        return minute;
    }
}

int convert_second(int minute)
{
    if (minute <= 0)
    {
        return 0;
    }
    else
    {
        return minute * 60;
    }
}