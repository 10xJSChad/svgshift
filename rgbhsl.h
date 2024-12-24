#pragma once


#define clamp(val, min, max) (val < min ? min : val > max ? max : val)


struct RGB {
    int r;
    int g;
    int b;
};


struct HSL {
    float h;
    float s;
    float l;
};


float
f_round(float x)
{
    int x_int = x;
    x -= x_int;

    if (x >= 0.5f)
        x = 1;
    else
        x = 0;

    return x + x_int;
}


float
f_max(float r,
      float g,
      float b)
{
    float max;

    max = r   > g ? r   : g;
    max = max > b ? max : b;

    return max;
}


float
f_min(float r,
      float g,
      float b)
{
    float min;

    min = r   < g ? r   : g;
    min = min < b ? min : b;

    return min;
}


int
hex_to_decimal(char* hex_str)
{
    char* endptr;
    return strtol(hex_str, &endptr, 16);
}


struct RGB
color_code_to_RGB(char* color_code_start)
{
    int i;
    char curr[3] = { 'A', 'B', '\0' };
    struct RGB new_color;

    for (i = 0; i < 6; i += 2) {
        curr[0] = color_code_start[i];
        curr[1] = color_code_start[i + 1];

        switch (i)
        {
            case 0:
                new_color.r = hex_to_decimal(curr);
                break;

            case 2:
                new_color.g = hex_to_decimal(curr);
                break;

            case 4:
                new_color.b = hex_to_decimal(curr);
                break;
        }
    }

    return new_color;
}


struct HSL
RGB_to_HSL(struct RGB rgb)
{
    struct HSL hsl;

    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float max  = f_max(r, g, b);
    float min  = f_min(r, g, b);
    float diff = max - min;

    hsl.s = hsl.l = (max + min) / 2;

    if (max == min) {
        hsl.h = hsl.s = 0;
    } else {
        /* No switch()-ing with floats, it was this or else-ifs (or gotos!) */
        do {
            if (max == r) {
                hsl.h = (g - b) / diff + (g < b ? 6 : 0);
                break;
            }
            if (max == g) {
                hsl.h = (b - r) / diff + 2;
                break;
            }
            if (max == b) {
                hsl.h = (r - g) / diff + 4;
                break;
            }
        } while(0);

        hsl.s = hsl.l > 0.5 ? diff / (2 - max - min) : diff / (max + min);
        hsl.h = hsl.h / 6;
    }

    hsl.h = f_round(hsl.h * 360);
    hsl.s = f_round(hsl.s * 100);
    hsl.l = f_round(hsl.l * 100);

    return hsl;
}


/* Taken from https://gist.github.com/ciembor/1494530 */
float
hue_to_RGB(float p,
           float q,
           float t)
{
    if (t < 0)
        t += 1;
    if (t > 1)
        t -= 1;
    if (t < 1./6)
        return p + (q - p) * 6 * t;
    if (t < 1./2)
        return q;
    if (t < 2./3)
        return p + (q - p) * (2./3 - t) * 6;

    return p;
}


/* Taken from https://gist.github.com/ciembor/1494530 */
struct RGB
HSL_to_RGB(struct HSL hsl)
{
    struct RGB rgb;
    float q, p;

    float h = hsl.h / 360;
    float s = hsl.s / 100;
    float l = hsl.l / 100;

    if (s == 0) {
        rgb.r = rgb.g = rgb.b = (l * 255);
    } else {
        q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        p = 2 * l - q;
        rgb.r = hue_to_RGB(p, q, h + 1./3) * 255;
        rgb.g = hue_to_RGB(p, q, h) * 255;
        rgb.b = hue_to_RGB(p, q, h - 1./3) * 255;
    }

    return rgb;
}


struct RGB
RGB_clamp(struct RGB rgb)
{
    rgb.r = clamp(rgb.r, 0, 255);
    rgb.g = clamp(rgb.g, 0, 255);
    rgb.b = clamp(rgb.b, 0, 255);

    return rgb;
}


struct HSL
HSL_clamp(struct HSL hsl)
{
    hsl.h = clamp(hsl.h, 0, 360);
    hsl.s = clamp(hsl.s, 0, 100);
    hsl.l = clamp(hsl.l, 0, 100);

    return hsl;
}


struct RGB
RGB_greyscale(struct RGB rgb) {
    int grayscale = (int) (0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b);
    return (struct RGB) {grayscale, grayscale, grayscale};
}
