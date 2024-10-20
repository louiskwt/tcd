#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include "utils.h"

// redefine assert to a function the returns a boolean
#ifdef assert
#undef assert
#endif
#define assert(x) (result = result && (x))

static int result;

static int expected_code;
static int should_exit;
static jmp_buf jump_env; // for holding the test case

static int done;
static int num_tests;
static int tests_passed;

void test_start(char *name);
void test_end();

// stub function to control how to handle exit and changing test cases
void exit(int code);

// test cases
void test_whole_number_minute_input();
void test_hour_input();
void test_fraction_input();

void test_start(char *name)
{
    num_tests++;
    result = 1;
    printf("-- Testing %s ...", name);
}

void test_end()
{
    if (result) tests_passed++;
    printf("%s\n", result ? "success" : "fail");
}

void exit(int code)
{
    if (!done)
    {
        assert(should_exit == 1);
        assert(expected_code == code);
        longjmp(jump_env, 1);
    }
    else
    {
        _exit(code);
    }
}

void test_whole_number_minute_input()
{
    int jmp_ravl;
    int input = 1;
    int total_second = input * 60;
    int second;
    int hour;
    int min;

    test_start("test_whole_number_minute");
    should_exit = 0;
    if (!(jmp_ravl=setjmp(jump_env)))
    {
        min = convert_minute(total_second);
        second = convert_second(total_second);
        hour = convert_hour(total_second);
    }
    assert(jmp_ravl == 0);
    assert(second == 0);
    assert(min == 1);
    assert(hour == 0);
    test_end();
    printf("...h: %i, m: %i, s: %i\n", hour, min, second);
}
void test_hour_input()
{
    int jmp_ravl;
    int input = atof("120");
    int total_second = input * 60;
    int second;
    int hour;
    int min;

    test_start("test_whole_number_minute");
    should_exit = 0;
    if (!(jmp_ravl=setjmp(jump_env)))
    {
        min = convert_minute(total_second);
        second = convert_second(total_second);
        hour = convert_hour(total_second);
    }
    assert(jmp_ravl == 0);
    assert(second == 0);
    assert(min == 0);
    assert(hour == 2);
    test_end();
    printf("...h: %i, m: %i, s: %i\n", hour, min, second);
}

void test_fraction_input()
{
    int jmp_ravl;
    float input = 0.1;
    int total_second = input * 60;
    int second;
    int hour;
    int min;

    test_start("test_whole_number_minute");
    should_exit = 0;
    if (!(jmp_ravl=setjmp(jump_env)))
    {
        min = convert_minute(total_second);
        second = convert_second(total_second);
        hour = convert_hour(total_second);
    }
    assert(jmp_ravl == 0);
    assert(second == 6);
    assert(min == 0);
    assert(hour == 0);
    test_end();
    printf("...h: %i, m: %i, s: %i\n", hour, min, second);
}

int main()
{
    num_tests = 0;
    tests_passed = 0;
    done = 0;
    test_whole_number_minute_input();
    test_hour_input();
    test_fraction_input();
    printf("Total tests passed: %d | Total tests failed: %d\n", tests_passed, num_tests - tests_passed);
    done = 1;
    return !(tests_passed == num_tests);
}