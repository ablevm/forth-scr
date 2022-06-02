#!/bin/sh

set -e

test() {
    ./scr -rsl example.scr | ./scr -"$1" | cmp - test/test."$1"
}

test l
test s
test d
test ls
test ds
test lsd
