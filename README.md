
# Linux Utility template

This is a template for a Linux command-line utility which to rename files using global patterns. The key features are:
- works on all the files from the current directory ;
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
- `rn -p "%_%" -r "%-%"` will replace all the underscores by dashes.

The [complete guide](tests/USERGUIDE.md) is UNDER WORK also...


## Tips & tricks

- ...