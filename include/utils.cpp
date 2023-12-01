#include "utils.h"

size_t get_file_size (FILE *stream)
{
    my_assert (stream != NULL);

    size_t start = ftell (stream);
    fseek (stream, start, SEEK_END);
    size_t size_file = ftell (stream);
    rewind (stream);

    return size_file;
}

char *get_str (FILE *stream)
{
    my_assert (stream != NULL);

    char ch = 0;

    size_t len = 0;
    size_t str_len = 10;

    char *str = (char *) calloc (str_len, sizeof (char));
    my_assert (str != NULL);

    while (!isspace (ch = (char) fgetc (stream)))
    {
        str[len++] = ch;
        
        if (len == str_len)
        {
            str = (char *) realloc (str, str_len * 2);
            my_assert (str != NULL);

            str_len *= 2;
        }
    }

    str[len++] = '\0';
    str = (char *) realloc (str, len);
    my_assert (str != NULL);

    return str;
}

bool is_zero (const double value)
{
    my_assert (isfinite(value));

    return (fabs (value) < EPSILON);
}

bool compare_number (const double value_1, const double value_2)
{
    my_assert (isfinite (value_1));

    my_assert (isfinite (value_2));

    return ((value_1 - value_2) > EPSILON);
}

void clean_buffer ()
{
    int ch = 0;

    while (((ch = getchar ()) != '\n') && (ch != EOF)) {}
}