#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <windows.h>

#include "xorshift.h"

#define DECK_SIZE 52

typedef enum
{
    XORSHIFT = 2,
    WELL512A
} Algos;

char algo_name[255];

void now(char *out, int n);
void shuffle(int *deck, unsigned (*rand_ptr)(void));

void builtin_rand_init();
unsigned builtin_rand();

struct xorshift32_state xorshift_state;
void xorshift_init();
unsigned xorshift();

int main(void)
{
    puts("\nShuffle Card Deck CLI");
    puts("---------------------------\n");

    int algo_id = 0;
    printf("Enter algorithm to test (1 - built-in random, 2 - Xorshift, 3 - WELL512a): ");
    scanf("%d", &algo_id);

    if (algo_id < 0 || algo_id > 3)
    {
        puts("Wrong selection!");
        EXIT_FAILURE;
    }

    // pointer to a random function
    unsigned (*rand_ptr)(void);
    switch (algo_id)
    {
    case XORSHIFT:
        strcpy(algo_name, "xosrhift");
        xorshift_init();
        rand_ptr = &xorshift;
        break;
    case WELL512A:
    default:
        strcpy(algo_name, "built-in");
        builtin_rand_init();
        rand_ptr = &builtin_rand;
    }

    int rounds;
    printf("Enter number of rounds (1 - 1000000): ");
    scanf("%d", &rounds);

    if (rounds < 1 || rounds > 1000000)
    {
        puts("Wrong number of rounds!");
        return EXIT_FAILURE;
    }

    int deck[DECK_SIZE];
    for (int i = 0; i < DECK_SIZE; i++)
    {
        deck[i] = i + 1;
    }

    // printf("\nfirst card = %i, last card = %i\n", deck[0], deck[51]);

    char start_str[12] = {0};
    now(start_str, sizeof(start_str));

    // https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9
    unsigned long long start_ms = GetTickCount();

    for (int i = 0; i < rounds; i++)
    {
        // Calling only rand() is super fast, so adding actual shuffle makes rounds
        // more "long running" (at least for large rounds)
        shuffle(deck, rand_ptr);
    }

    unsigned long long end_ms = GetTickCount();

    char end_str[12] = {0};
    now(end_str, sizeof(end_str));

    // printf("\nfirst card = %i, last card = %i\n", deck[0], deck[51]);

    puts("");
    puts(" Algorithm | Time started |  Time ended  | Total time ");
    puts("-----------+--------------+--------------+------------");
    printf(" %9s | %12s | %12s | %8llums \n\n", algo_name, start_str, end_str, end_ms - start_ms);
}

// See https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/time-time32-time64?view=msvc-160
void now(char *out_str, int n)
{
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);
    strftime(out_str, n, "%X", timeinfo);
}

// See https://stackoverflow.com/questions/42321370/fisher-yates-shuffling-algorithm-in-c
void shuffle(int *deck, unsigned (*rand_ptr)(void))
{
    unsigned r, tmp;
    for (int i = DECK_SIZE - 1; i > 0; i--)
    {
        r = (*rand_ptr)() % (i + 1);
        tmp = deck[r];
        deck[r] = deck[i];
        deck[i] = tmp;
    }
}

void builtin_rand_init()
{
    time_t t;
    srand((unsigned)time(&t));
}

unsigned builtin_rand()
{
    return (unsigned)rand();
}

void xorshift_init()
{
    time_t t;
    xorshift_state.a = (unsigned)time(&t);
}

unsigned xorshift()
{
    return xorshift32(&xorshift_state);
}