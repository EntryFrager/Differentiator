#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"

#define compare(first_value, second_value) compare_number (first_value, second_value, code_error)

#define FOPEN_(fp, fp_name, mode)               \
    FILE *fp = fopen (fp_name, mode);           \
    my_assert (fp != NULL, ERR_FOPEN);

#define FCLOSE_(fp)                             \
    my_assert (fclose (fp) == 0, ERR_FCLOSE);

const double EPSILON = 1e-10;

size_t get_file_size (FILE *stream, int *code_error);

char *get_str (FILE *stream, int *code_error);

char *skip_isspace (char *str, int *code_error);

int compare_number (const double value_1, const double value_2, int *code_error);

void clean_buffer ();

#endif //UTILS_H