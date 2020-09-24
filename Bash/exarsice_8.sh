#!/bin/bash

find -mindepth 1 -type f -links +1 -printf '%n %f\n' | sort -rn | head -5 | cut -d" " -f 2
