#include <stdio.h>
#include <time.h>

int main (void)
{
    // initial set up
    clock_t begin = clock();
    int count = 60;
    double time_spent = 0;

    // count down logic
    do
    {
        printf("Counting down...  %f\n", time_spent);
        time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    }
    while(time_spent <= 60);

    // return when finished
    return 0;
}