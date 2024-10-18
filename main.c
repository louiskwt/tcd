#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*
 TODO: 1) fix 1 minute count down (done)
       2 ) center the count down (done)
       3) play sound when doen 
       4 ) display different font styles 
       5 ) accept input 
       6 ) write some test
*/


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
    // get terminal size to center the countdown
    struct winsize terminal_window;
    ioctl(STDERR_FILENO, TIOCGWINSZ, &terminal_window);
    int t_height = terminal_window.ws_row;
    int t_width = terminal_window.ws_col;
    
    // initial set up
    int total_second = atof(argv[1]) * 60;
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
        // dynamic resizing
        ioctl(STDERR_FILENO, TIOCGWINSZ, &terminal_window);
        if (t_width != terminal_window.ws_col || t_height != terminal_window.ws_row)
        {
            t_height = terminal_window.ws_row;
            t_width = terminal_window.ws_col;
            clear();
        }

        mvprintw(floor(t_height / 2), floor(t_width / 2) - 6, "%02i : %02i : %02i", hour, minute, second);
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
    clear();
    mvprintw(floor(t_height / 2), floor(t_width / 2) - 6, "press any key to exit...");
    refresh();
    getch();
    endwin();
    printf("total second: %i\n", total_second);
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
    if (second < 60)
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
