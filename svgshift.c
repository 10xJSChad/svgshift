#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rgbhsl.h"
#include "color.h"


#define OPERATION_SET 0
#define OPERATION_ADD 1
#define OPERATION_SUB 2
#define OPERATION_GREYSCALE 3


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
next_color_code(char* str,
                int*  color_code_length)
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
                *color_code_length = 6;
                return str - 5;

            case 3:
                if (!isxdigit(*(str + 1))) {
                    *color_code_length = 3;
                    return str - 2;
                }

            /* Fallthrough */

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


void
allocate_and_read(FILE* file,
                  char** dest_buffer,
                  char** empty_buffer)
{
    size_t file_size = 0;
    size_t read_bytes;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *dest_buffer = calloc(1, (file_size * 2) + 1);
    *empty_buffer = calloc(1, (file_size * 2) + 1);

    if (*dest_buffer == NULL || *empty_buffer == NULL) {
        perror("Error allocating memory");
        exit(1);
    }

    read_bytes = fread(*dest_buffer, 1, file_size, file);

    if (read_bytes != file_size) {
        perror("Error reading file");
        exit(1);
    }
}


int
manipulate(FILE* file,
           struct Color (*func)(struct Color))
{
    char*   left;
    char*   right;
    char*   src_buffer;
    char*   dest_buffer;
    char    new_color_code_ptr[7] = { 0, };
    int     color_code_length;
    allocate_and_read(file, &src_buffer, &dest_buffer);


    left = src_buffer;
    right = src_buffer;
    while ((right = next_color_code(right, &color_code_length))) {
        struct Color new_color = color_code_to_Color(right, color_code_length);
        Color_to_color_code(func(new_color), new_color_code_ptr);

        strncat(dest_buffer, left, right - left);
        strncat(dest_buffer, new_color_code_ptr, 6);
        left = right + color_code_length;
    }

    strncat(dest_buffer, left, right - left);
    printf("%s", dest_buffer);
    return 1;
}


void
print_usage_and_exit(char* usage)
{
    fprintf(stderr, "%s\n", usage);
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


/*
 * Generic debug function, replace with whatever you want during development.
 */
void
debug_function(void)
{
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

        case OPERATION_GREYSCALE:
            color.rgb = RGB_greyscale(color.rgb);
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
    "   svgshift [set|add|sub][rgb|hsl] [VALUES] [FILE]\n"
    "   svgshift greyscale [FILE]\n\n"

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
    "   files that have numeric identifiers, if you want to read a file that has\n"
    "   one, you _must_ supply all 3 RGB/HSL values, otherwise it will read your\n"
    "   filename as an RGB/HSL value, and complain that there is no input file.\n\n"

    "EXAMPLES:\n"
    "   svgshift setrgb 255 x x icon.svg | Set the red value of every color\n"
    "   to 255, leaving green and blue unaffected.\n\n"

    "   svgshift setrgb 255 0 0 icon.svg | Make every color in icon.svg fully red, \n"
    "   overwriting green and blue with 0.\n\n"

    "   svgshift sethsl x 0 x icon.svg | Set the saturation to 0, leaving \n"
    "   hue and lightness unaffected.\n");


    #ifndef DEBUG_MODE
    int i;
    FILE* file;

    if (argc == 1)
        print_usage_and_exit(usage_str);

    if (strncmp(argv[1], "set", 3) == 0)        operation = OPERATION_SET;
    if (strncmp(argv[1], "add", 3) == 0)        operation = OPERATION_ADD;
    if (strncmp(argv[1], "sub", 3) == 0)        operation = OPERATION_SUB;
    if (strncmp(argv[1], "greyscale", 9) == 0)  operation = OPERATION_GREYSCALE;
    if (strncmp(argv[1], "grayscale", 9) == 0)  operation = OPERATION_GREYSCALE;

    if (operation == -1)
         print_usage_and_exit(usage_str);

    if (operation != OPERATION_GREYSCALE) {
        if (strncmp(argv[1]+3, "rgb", 3) == 0) is_rgb = 1;
        if (strncmp(argv[1]+3, "hsl", 3) == 0) is_rgb = 0;

        if (is_rgb == -1)
            print_usage_and_exit(usage_str);
    }

    i = 0;
    argv += 2;
    while (*argv) {
        if (is_numeric(*argv)) {
            values[i] = atoi(*argv);
            fields_changed[i++] = 1;
        } else if (**argv == 'x') {
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

    if (file == NULL) {
        printf("Could not open file %s\n", *argv);
        exit(1);
    }

    manipulate(file, operation_function);
    #else
    debug_function();
    #endif
    return 0;
}