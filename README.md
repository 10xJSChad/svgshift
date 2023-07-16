### svgshift
svgshift performs quick and easy color manipulation on svg files, because
editing those manually required some software I didn't have. It can also be used to manipulate the colors of just about any file
with a format that stores its color information in plaintext as hex colors.

------

### Preview
https://github.com/10xJSChad/svgshift/assets/48174610/5f52c904-810f-4b54-9726-d10f02bf72d7

(Note that this preview shows a bug where HSL operations would make the greys become significantly darker, this is now fixed.)

------

### Limitations
svgshift does _NOT_ perform any file writing whatsoever, it prints the
adjusted contents of the file to stdout, which then has to be redirected
to an output file.

Additionally, the argument parser is very lazy and does not at all care for
files that have numeric identifiers, if you are trying to read a file that has
one, you _must_ supply all 3 RGB/HSL values, otherwise it will read your
filename as an RGB/HSL value, and complain that there is no input file.

### Issues
There are slight inaccuracies when performing conversions between RGB, HSL, and hex. It will likely increment all RGB values by one, this is something I am more or less biologically incapable of noticing, so I don't care much about it, it's definitely caused by the liberal use of int-float-int conversions though.

The program works fine for me as is, and has successfully done the one thing I needed it to do, which was shift the hue on some of the svgs used by my Plasma theme, so whatever problems it still has are not a great concern for me.

### Contributing
Feel free to send PRs with whatever, I'll accept just about any change if it does something meaningful.

### Building svgshift
It should compile on literally anything, there are no external dependencies, and it's even C89 compatible (ok, technically the usage string is *slightly* longer than C89 compilers are required to support, but still!).


### Using svgshift
Once it's compiled, just run it once and a usage message should appear guiding you towards great success in color shifting svg files.
