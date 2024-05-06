
# Rename

This is a Linux command-line utility which recursively rename files and directories using shell-like patterns (with `%`instead of `*`). Can be helpful to rename files from a camera, or to rename directories from a backup. The key features are:
- works on all the files and directories recursively ;
- use simple patterns with % as a wildcard.

## Credits

- The file `argparse.hpp` 3.0 has been downloaded from https://github.com/p-ranav/argparse
- Developed with the help of Docker and [this recipe](https://github.com/chavid/DevScripts/blob/main/Cpp20/Dockerfile).
- The tests are handled with [oval script](https://github.com/chavid/DevScripts/blob/main/bin/oval.py) .


## Requirements

This is all done for Linux and a bash terminal.
It can be compiled with any compiler supporting C++20.


## Fast and dirty guide

See `rn --help`.

Examples :
- `rn "%_%" "%-%"` will replace all the underscores by dashes.

The [complete guide](tests/USERGUIDE.md) is UNDER WORK also...


## Tips & tricks

- the `%` wildcard is replace with `.*` so to form an ECMAScript regular expression.
- all other characters are "escaped" so that they just mean themselves.
- the transformation on the file name is reapplied as long as the input pattern is matching.
