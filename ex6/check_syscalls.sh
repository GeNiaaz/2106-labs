#!/bin/bash

####################
# Lab 1 Exercise 6
# Name: Muhammad Niaaz Wahab
# Student No: A0200161E
# Lab Group: B04
####################

echo "Printing system call report"

# Compile file
gcc -std=c99 pid_checker.c -o ex6

# Use strace to get report

strace -c ./ex6