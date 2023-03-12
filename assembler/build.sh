#!/bin/bash

set -xe

flags="-Werror=declaration-after-statement -g"

if [ "$1" = "prod" ]; then
    flags=${flags/-g/-pedantic -Wall -Wextra -std=c99 -O2}
fi

gcc $flags -o asm *.c
