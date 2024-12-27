#pragma once


struct Color {
    struct RGB rgb;
    struct HSL hsl;
};


struct Color
color_code_to_Color(char* color_code_start,
                    int  color_code_length)
{
    struct Color new_color;

    if (color_code_length == 6)
        new_color.rgb = color_code_to_RGB(color_code_start);
    else
        new_color.rgb = color_code_to_short_RGB(color_code_start);

    new_color.hsl = RGB_to_HSL(new_color.rgb);

    return new_color;
}


void
Color_to_color_code(struct Color color,
                    char* dest)
{
    sprintf(dest, "%.2x%.2x%.2x", color.rgb.r, color.rgb.g, color.rgb.b);
}