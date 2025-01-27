#pragma once
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

// Structure to hold progress bar information
typedef struct {
    int total;
    int count;
    int size;
    double average_time;
    double last_time;
    double total_time;
} tqdm;

int init_tqdm(tqdm *progress, int total, int size);
void update_tqdm(tqdm *progress, int x);
