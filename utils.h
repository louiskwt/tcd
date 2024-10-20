#include <math.h>

int convert_hour(int minute);
int convert_minute(int second);
int convert_second(int second);


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
    int min = round(second / 60);
    return min > 59 ? 0 : min;
}

int convert_second(int second)
{
   return second % 60;
}
