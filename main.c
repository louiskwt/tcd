#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ao/ao.h>
#include <mpg123.h>


/*
 TODO: 1) fix 1 minute count down (done)
       2 ) center the count down (done)
       3) play sound when doen 
       4 ) display different font styles 
       5 ) accept input 
       6 ) write some test
*/

#define EIGHT_BITS 8;

int convert_hour(int minute);
int convert_minute(int second);
int convert_second(int second);
int play_sound(void);

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
    play_sound();
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