#include "utils.h"

size_t get_file_size (FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    size_t start = ftell (stream);
    fseek (stream, start, SEEK_END);
    size_t size_file = ftell (stream);
    rewind (stream);

    return size_file;
}

char *get_str (FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    char ch = 0;

    size_t len = 0;
    size_t str_len = 10;

    char *str = (char *) calloc (str_len, sizeof (char));
    my_assert (str != NULL, ERR_MEM);

    while (!isspace (ch = (char) fgetc (stream)))
    {
        str[len++] = ch;
        
        if (len == str_len)
        {
            str = (char *) realloc (str, str_len * 2);
            my_assert (str != NULL, ERR_MEM);

            str_len *= 2;
        }
    }

    str[len++] = '\0';
    str = (char *) realloc (str, len);
    my_assert (str != NULL, ERR_MEM);

    return str;
}

char *skip_isspace (char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    size_t len = strlen (str);
    char *new_str = (char *) calloc (len, sizeof (char));
    my_assert (new_str != NULL, ERR_MEM);

    size_t pos = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (!isspace (str[i]))
        {
            new_str[pos++] = str[i];
        }
    }

    new_str = (char *) realloc (new_str, (pos + 1) * sizeof (char));
    my_assert (new_str != NULL, ERR_MEM);

    new_str[pos] = '\0';

    free (str);

    return new_str;
}

bool is_zero (const double value, int *code_error)
{
    my_assert (isfinite(value), ERR_NAN);

    return (fabs (value) < EPSILON);
}

int compare_number (const double value_1, const double value_2, int *code_error)
{
    my_assert (isfinite (value_1), ERR_NAN);

    my_assert (isfinite (value_2), ERR_NAN);

    if ((value_1 - value_2) > EPSILON)
    {
        return 1;
    }
    else if ((value_1 - value_2) < (-1) * EPSILON)
    {
        return -1;
    }

    return 0;
}

void clean_buffer ()
{
    int ch = 0;

    while (((ch = getchar ()) != '\n') && (ch != EOF)) {}
}