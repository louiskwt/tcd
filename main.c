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

#define EIGHT_BITS 8
#define ROW 5
#define COL 5
#define MESSAGE_OFFSET 17
#define DISPLAY_OFFSET 18

// struct for text [row][col] 5 x 5 #
typedef char TEXT[ROW][COL];

static float input_min = 0;
static int total_second_from_user_input = 0;

int play_sound(void);
void draw_text(int num, int x, int y);
void print_digit(int digit, int x, int y);

int main (int argc, char* argv[])
{
    // check command line argument
    if (argc > 2)
    {
        printf("Usage: ./tcd MINUTES or leave it blank and enter the time yourself.\n");
        return 1;
    }

    if (argc == 2)
    {
        input_min = atof(argv[1]);

        if (input_min == 0)
        {
            printf("Invalid minute input. Please enter a number for the minute.");
            return 1;
        }

        if (round(input_min / 60) >= 100)
        {
            printf("Exceed the mixmum minute (6000)! Please try again with a smaller number.\n");
            return 1;
        }
    }
    else
    {
        int user_input_hr;
        int user_input_min;
        int user_input_second;

        printf("Enter a number for hour: ");
        if (scanf("%d", &user_input_hr) != 1 || user_input_hr > 99)
        {
            printf("Invalid number for hour. Please enter a number between 0 and 99.\n");
            return 1;
        }

        printf("Enter a number for minute: ");
        if (scanf("%d", &user_input_min) != 1 || user_input_min >= 60)
        {
            printf("Invalid number for minute. Please enter a number between 0 and 59.\n");
            return 1;
        }

        printf("Enter a number for second: ");
        if (scanf("%d", &user_input_second) != 1 || user_input_second >= 60)
        {
            printf("Invalid number for second. Please enter a number between 0 and 59.\n");
            return 1;
        }

        total_second_from_user_input = convert_user_input_to_second(user_input_hr, user_input_min, user_input_second);
    }

    // get terminal size to center the countdown
    struct winsize terminal_window;
    ioctl(STDERR_FILENO, TIOCGWINSZ, &terminal_window);
    int t_height = terminal_window.ws_row;
    int t_width = terminal_window.ws_col;

    // initial set up
    int total_second = total_second_from_user_input == 0 ? round(input_min * 60) : total_second_from_user_input;

    int minute = convert_minute(total_second);
    int hour = convert_hour(total_second);
    int second = convert_second(total_second);

    double time_spent = 0;
    double prev_time = 0;
    // start ncurses window
    initscr();
    curs_set(0); // remove cursor

    if (!has_colors())
    {
        mvprintw(floor(t_height / 2), floor(t_width / 2) - MESSAGE_OFFSET, "Color not supported. Press any key to continue."); // center the message by its half lenght which is 15
        getch();
    }
    else
    {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_GREEN, -1);
        init_pair(2, COLOR_RED, -1);
        attrset(COLOR_PAIR(1));
    }

    refresh();
    
    clock_t begin = clock();
    // count down logic
    while(total_second >= 0)
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
    
    clear();
    if (has_colors())
    {
        attrset(COLOR_PAIR(2));
    }
    mvprintw(floor(t_height / 2), floor(t_width / 2) - MESSAGE_OFFSET, "Time's up! Press ctrl + c to exit."); // center the message by its half lenght which is 15
    refresh();
    play_sound();
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
    buffer = NULL;

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
        mvprintw(floor(y / 2) + i,  floor(x / 2) - DISPLAY_OFFSET, t[i]); // center the number on window by its half length which is 18
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
