# svgshift

svgshift performs quick and easy color manipulation on svg files, because
editing those manually required some software I didn't have. It can also be used to manipulate the colors of just about any file
with a format that stores its color information in plaintext as hex colors.

------

## Preview

<https://github.com/10xJSChad/svgshift/assets/48174610/5f52c904-810f-4b54-9726-d10f02bf72d7>

(Note that this preview shows a bug where HSL operations would make the greys become significantly darker, this is now fixed.)

------

## Limitations

svgshift does _NOT_ perform any file writing whatsoever, it prints the
adjusted contents of the file to stdout, which then has to be redirected
to an output file.

Additionally, the argument parser is very lazy and does not at all care for
files that have numeric identifiers, if you are trying to read a file that has
one, you _must_ supply all 3 RGB/HSL values, otherwise it will read your
filename as an RGB/HSL value, and complain that there is no input file.

## Issues

There are slight inaccuracies when performing conversions between RGB, HSL, and hex. It will likely increment all RGB values by one, this is something I am more or less biologically incapable of noticing, so I don't care much about it, it's definitely caused by the liberal use of int-float-int conversions though.

The program works fine for me as is, and has successfully done the one thing I needed it to do, which was shift the hue on some of the svgs used by my Plasma theme, so whatever problems it still has are not a great concern for me.

## Contributing

Feel free to send PRs with whatever, I'll accept just about any change if it does something meaningful.

## Prerequisites

It should compile on literally anything, there are no external dependencies, and it's even C89 compatible (ok, technically the usage string is _slightly_ longer than C89 compilers are required to support, but still!).

I recommend using the gcc compiler to compile it, but it should work with any C compiler.

### GCC installation on Windows

[![How to Install GCC Compiler Tools in Windows 11 (C/C++)](https://markdown-videos-api.jorgenkh.no/url?url=https%3A%2F%2Fwww.youtube.com%2Fwatch%3Fv%3DGxFiUEO_3zM)](https://www.youtube.com/watch?v=GxFiUEO_3zM)

### GCC installation on Linux

[![Super Easy C / C++ Tools Install on Linux - Create and Compile Your First Program with GCC / G++.](https://markdown-videos-api.jorgenkh.no/url?url=https%3A%2F%2Fwww.youtube.com%2Fwatch%3Fv%3D4e7pa6Pf3VQ)](https://www.youtube.com/watch?v=4e7pa6Pf3VQ)

## Building svgshift

For example you can compile it using gcc with the following command:

```sh
gcc svgshift.c -o svgshift
```

## Using svgshift

Once it's compiled, just run it once and a usage message should appear guiding you towards great success in color shifting svg files.
Here's an example command that will create a greyscale version of an image.

```sh
.\svgshift sethsl x 0 x colorful.svg > greyscaled.svg # windows
svgshift sethsl x 0 x colorful.svg > greyscaled.svg # linux or mac
```
Don't pipe the output into the source svg file, this will result in it being wiped.
