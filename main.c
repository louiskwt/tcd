#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <ao/ao.h>
#include <mpg123.h>
#include "utils.h"

/*
 TODO: 1) fix 1 minute count down (done)
       2 ) center the count down (done)
       3) play sound when count down end (done)
       4 ) display different font styles (done) 
       5 ) accept input 
       6 ) write some tests (done)
*/

#define EIGHT_BITS 8
#define ROW 5
#define COL 5

// struct for text [row][col] 5 x 5 #
typedef char TEXT[ROW][COL];

int play_sound(void);
void draw_text(int num, int x, int y);
void print_digit(int digit, int x, int y);

int main (int argc, char* argv[])
{
    // check command line argument
    if (argc < 2 || argc > 2)
    {
        printf("Usage: ./tcd MINUTES\n");
        return 1;
    }

    int input_min = atof(argv[1]);
    if (round(input_min / 60) >= 100)
    {
        printf("Exceed the mixmum minute (6000)! Please try again with a smaller number.\n");
        return 1;
    }

    // get terminal size to center the countdown
    struct winsize terminal_window;
    ioctl(STDERR_FILENO, TIOCGWINSZ, &terminal_window);
    int t_height = terminal_window.ws_row;
    int t_width = terminal_window.ws_col;

    // initial set up
    int total_second = input_min * 60;

    int minute = convert_minute(total_second);
    int hour = convert_hour(total_second);
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
        int offset_x = 0;
        // dynamic resizing
        ioctl(STDERR_FILENO, TIOCGWINSZ, &terminal_window);
        if (t_width != terminal_window.ws_col || t_height != terminal_window.ws_row)
        {
            t_height = terminal_window.ws_row;
            t_width = terminal_window.ws_col;
            clear();
        }

        print_digit(hour, t_width, t_height);
        offset_x += 20;
        draw_text(9999, t_width + offset_x, t_height);
        offset_x += 10;
        print_digit(minute, t_width + offset_x, t_height);
        offset_x += 20;
        draw_text(9999, t_width + offset_x, t_height);
        offset_x += 10;
        print_digit(second, t_width + offset_x, t_height);

        time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC; // in second
        
        if (time_spent - prev_time >= 1)
        {
            total_second--;
            minute = convert_minute(total_second);
            hour = convert_hour(total_second);
            second = convert_second(total_second);
            prev_time++;
        }
    }
    while(total_second >= 0);

    // wait for key press and close window
    clear();
    mvprintw(floor(t_height / 2), floor(t_width / 2) - 15, "Times up! Press any key to exit."); // center the message by its half lenght which is 15
    refresh();
    play_sound();
    getch();
    endwin();
    return 0;
}


int play_sound()
{
    // initializations
    mpg123_handle *m_handle;
    char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    ao_device *device;

    ao_sample_format format;
    int channels, encoding;
    long rate;

    ao_initialize();
    int driver = ao_default_driver_id();
    mpg123_init();

    m_handle = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(m_handle);
    buffer = (char*) malloc(buffer_size * sizeof(char));

    // open the file and get decoding format
    mpg123_open(m_handle, "sound.mp3");
    mpg123_getformat(m_handle, &rate, &channels, &encoding);

    // set the output sound format
    format.bits = mpg123_encsize(encoding) * EIGHT_BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    device = ao_open_live(driver, &format, NULL);

    // decode and play
    while (mpg123_read(m_handle, buffer, buffer_size, &done) == MPG123_OK)
    {
        ao_play(device, buffer, done);
    }
 
    free(buffer);
    ao_close(device);
    mpg123_close(m_handle);
    mpg123_delete(m_handle);
    mpg123_exit();
    ao_shutdown();

    return 0;
}

void draw_text(int num, int x, int y)
{
    // struct for text [row][col] 5 x 5 #
    TEXT t;
    switch (num)
    {
    case 1:
        strcpy(t[0], "  # ");
        strcpy(t[1], "### ");
        strcpy(t[2], "  # ");
        strcpy(t[3], "  # ");
        strcpy(t[4], "####");
        break;
    case 2:
        strcpy(t[0], "####");
        strcpy(t[1], "   #");
        strcpy(t[2], "####");
        strcpy(t[3], "#   ");
        strcpy(t[4], "####");
        break;
    case 3:
        strcpy(t[0], "####");
        strcpy(t[1], "   #");
        strcpy(t[2], " ###");
        strcpy(t[3], "   #");
        strcpy(t[4], "####");
        break;
    case 4:
        strcpy(t[0], "#  #");
        strcpy(t[1], "#  #");
        strcpy(t[2], "####");
        strcpy(t[3], "   #");
        strcpy(t[4], "   #");
        break;
    case 5:
        strcpy(t[0], "####");
        strcpy(t[1], "#   ");
        strcpy(t[2], "####");
        strcpy(t[3], "   #");
        strcpy(t[4], "####");
        break;
    case 6:
        strcpy(t[0], "####");
        strcpy(t[1], "#   ");
        strcpy(t[2], "####");
        strcpy(t[3], "#  #");
        strcpy(t[4], "####");
        break;
    case 7:
        strcpy(t[0], "####");
        strcpy(t[1], "   #");
        strcpy(t[2], "   #");
        strcpy(t[3], "   #");
        strcpy(t[4], "   #");
        break;
    case 8:
        strcpy(t[0], "####");
        strcpy(t[1], "#  #");
        strcpy(t[2], "####");
        strcpy(t[3], "#  #");
        strcpy(t[4], "####");
        break;
    case 9:
        strcpy(t[0], "####");
        strcpy(t[1], "#  #");
        strcpy(t[2], "####");
        strcpy(t[3], "   #");
        strcpy(t[4], "####");
        break;
    case 0:
        strcpy(t[0], "####");
        strcpy(t[1], "#  #");
        strcpy(t[2], "#  #");
        strcpy(t[3], "#  #");
        strcpy(t[4], "####");
        break;
    default:
        strcpy(t[0], " ## ");
        strcpy(t[1], " ## ");
        strcpy(t[2], "    ");
        strcpy(t[3], " ## ");
        strcpy(t[4], " ## ");
        break;
    }
    for (int i = 0; i < ROW; i++)
    {
        mvprintw(floor(y / 2) + i,  floor(x / 2) - 18, t[i]); // center the number on window by its half length which is 18
        refresh();
    }
    return;
}

void print_digit(int digit, int x, int y)
{
    for (int i = 0; i < 2; i++)
    { 
    

        if (digit >= 10) 
        {
            int d = (int)(floor(digit / 10));
            draw_text(d, x + (i * 10), y);
            digit = digit % 10;
        } 
        else if (digit < 10 && i == 0)
        {
            draw_text(i, x + (i * 10), y);
        }
        else
        {
            draw_text(digit, x + (i * 10), y); 
        }  
    }
    return;
}

// +--- 3x5 figlet

                                        
// ###  #  ### ### # # ### ### ### ### ### 
// # # ##    #   # # # #   #     # # # # # 
// # #  #  ###  ## ### ### ###   # ### ### 
// # #  #  #     #   #   # # #   # # #   # 
// ### ### ### ###   # ### ###   # ### ### 
