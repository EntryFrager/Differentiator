#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#include "error.h"

#define FOPEN_(fp, fp_name, mode)               \
    FILE *fp = fopen (fp_name, mode);           \
    if (fp == NULL) {return ERR_FOPEN;}

#define FCLOSE_(fp)                             \
    if (fclose (fp) != 0) {return ERR_FCLOSE;} 

const double EPSILON = 1e-10;

size_t get_file_size (FILE *stream);

char *get_str (FILE *stream);

bool is_zero (const double value);

bool compare_number (const double value_1, const double value_2);

void clean_buffer ();

#endif //UTILS_H