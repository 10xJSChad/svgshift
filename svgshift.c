#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rgbhsl.h"
#include "color.h"


#define BUFFER_SIZE 2048

#define OPERATION_SET 0
#define OPERATION_ADD 1
#define OPERATION_SUB 2


/*
 * Avoid some ugly copy-paste.
 * (alternatively, avoid writing cleaner code :) )
 */
#define PASTE_OPERATION(math_op)                    \
    if (is_rgb) {                                   \
        if (fields_changed[0])                      \
            color.rgb.r math_op values[0];          \
        if (fields_changed[1])                      \
            color.rgb.g math_op values[1];          \
        if (fields_changed[2])                      \
            color.rgb.b math_op values[2];          \
        color.rgb = RGB_clamp(color.rgb);           \
    } else {                                        \
        if (fields_changed[0])                      \
            color.hsl.h math_op values[0];          \
        if (fields_changed[1])                      \
            color.hsl.s math_op values[1];          \
        if (fields_changed[2])                      \
            color.hsl.l math_op values[2];          \
        color.hsl = HSL_clamp(color.hsl);           \
        color.rgb = HSL_to_RGB(color.hsl);          \
    }


/* Globals because I'm feeling particularly lazy right now. */
int operation = -1;
int is_rgb    = -1;
int values[3]         = {0, 0, 0};
int fields_changed[3] = {0, 0, 0};


char*
next_color_code(char* str)
{
    int count = 0;
    while (*str) {
        switch (count)
        {
            case 0:
                if ((*str) == '#')
                    ++count;

                break;

            /* Counted 6 hex digits in a row + the leading octothorpe */
            case 6:
                return str - 5;

            default:
                if (isxdigit(*str))
                    ++count;
                else
                    count = 0;
        }
        ++str;
    }

    /* EOF reached without any color codes found */
    return NULL;
}


int
read_file_to_buffer(char* buffer,
                    FILE* file)
{
    int i;
    int read_bytes = fread(buffer, 1, BUFFER_SIZE, file);

    if (read_bytes == BUFFER_SIZE) {
        for (i = 0; i < 6; ++i) {
            /* make sure we're not splitting a color code, go back if we are */
            if (buffer[BUFFER_SIZE - i] == '#') {
                fseek(file, -i, SEEK_CUR);
                read_bytes -= i;
            }
        }
    }

    buffer[read_bytes] = '\0';
    return read_bytes;
}


int
manipulate(FILE* file,
           struct Color (*func)(struct Color))
{
    char*   color_code_ptr;
    char    buffer[BUFFER_SIZE + 1];
    char    new_color_code_ptr[7] = { 0, };


    while (read_file_to_buffer(buffer, file)) {
        color_code_ptr = buffer;

        while ( (color_code_ptr = next_color_code(color_code_ptr)) ) {
            struct Color new_color = func(color_code_to_Color(color_code_ptr));

            Color_to_color_code(new_color, new_color_code_ptr);
            strncpy(color_code_ptr, new_color_code_ptr, 6);
        }

        printf("%s", buffer);
    }

    return 1;
}


void
print_usage_and_exit(char* usage)
{
    printf("%s\n", usage);
    exit(1);
}


int
is_numeric(char* str)
{
    while (*str)
        if (!isdigit(*str))
            return 0;
        else
            ++str;

    return 1;
}


struct Color
operation_function(struct Color color)
{
    switch (operation)
    {
        case OPERATION_SET:
            PASTE_OPERATION(=);
            break;

        case OPERATION_ADD:
            PASTE_OPERATION(+=);
            break;

        case OPERATION_SUB:
            PASTE_OPERATION(-=);
            break;
    }

    return color;
}


int
main(int    argc,
     char **argv)
{
    char* usage_str =
    ("USAGE:\n"
    "   svgshift [set|add|sub][rgb|hsl] [VALUES] [FILE]\n\n"

    "INFO:\n"
    "   svgshift performs quick and easy color manipulation on svg files, because\n"
    "   editing those manually required some software I didn't have.\n\n"

    "   It can also be used to manipulate the colors of just about any file\n"
    "   with a format that stores its color information in plaintext as hex colors.\n\n"

    "LIMITATIONS:\n"
    "   svgshift does _NOT_ perform any file writing whatsoever, it prints the\n"
    "   adjusted contents of the file to stdout, which then has to be redirected\n"
    "   to an output file.\n\n"

    "   Additionally, the argument parser is very lazy and does not at all care for\n"
    "   files that have numeric identifiers, if you are trying to read a file that has\n"
    "   one, you _must_ supply all 3 RGB/HSL values, otherwise it will read your\n"
    "   filename as an RGB/HSL value, and complain that there is no input file.\n\n"

    "EXAMPLES:\n"
    "   svgshift setrgb 255 icon.svg | Set the red value of every color in icon.svg\n"
    "   to 255, ignoring the green and blue channels.\n\n"

    "   svgshift setrgb 255 0 0 icon.svg | Make every color in icon.svg fully red, \n"
    "   overwriting the green and blue channels with 0.\n\n"

    "   hsl usage is identical.");


    int i;
    FILE* file;

    if (argc == 1)
        print_usage_and_exit(usage_str);

    if (strncmp(argv[1], "set", 3) == 0) operation = OPERATION_SET;
    if (strncmp(argv[1], "add", 3) == 0) operation = OPERATION_ADD;
    if (strncmp(argv[1], "sub", 3) == 0) operation = OPERATION_SUB;

    if (operation == -1)
         print_usage_and_exit(usage_str);

    if (strncmp(argv[1]+3, "rgb", 3) == 0) is_rgb = 1;
    if (strncmp(argv[1]+3, "hsl", 3) == 0) is_rgb = 0;

    if (is_rgb == -1)
         print_usage_and_exit(usage_str);

    i = 0;
    argv += 2;
    while (*argv) {
        if (is_numeric(*argv)) {
            values[i] = atoi(*argv);
            fields_changed[i] = 1;
            ++i;
        } else {
            break;
        }

        if (i >= 3) {
            ++argv;
            break;
        }

        ++argv;
    }

    if (*argv == NULL)
        print_usage_and_exit(usage_str);

    file = fopen(*argv, "r");
    manipulate(file, operation_function);

    return 0;
}