#include <math.h>

int convert_hour(int minute);
int convert_minute(int second);
int convert_second(int second);
int convert_user_input_to_second(int hour, int minute, int second);


int convert_hour(int second)
{
    int min = round(second / 60);
    return min < 60 ? 0 : round(min / 60);
}

int convert_minute(int second)
{
    if (second < 60) return 0;
    
    int min = round(second / 60);
    return min > 59 ? min % 60 : min;
}

int convert_second(int second)
{
   return second % 60;
}

int convert_user_input_to_second(int hour, int minute, int second)
{
    int second_from_hour = hour * 60 * 60;
    int second_from_minute = minute * 60;
    return second_from_hour + second_from_minute + second;
}