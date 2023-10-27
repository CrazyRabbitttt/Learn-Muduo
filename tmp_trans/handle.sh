#!/bin/bash

awk -F', ' '{
    for (i = 1; i <= NF; i++) {
        if ($i ~ /t=[0-9]+ms/ && int(substr($i, 3, length($i) - 4)) > 1000) {
            print;
            break;
        }
    }
}' $1

