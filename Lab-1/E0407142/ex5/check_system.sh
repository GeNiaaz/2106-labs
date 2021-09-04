#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Muhammad Niaaz Wahab
# Student No: A0200161E
# Lab Group: B04
####################

# Fill the below up
hostname=$(hostname)
machine_hardware=$(uname -s -p)
max_user_process_count=$(ulimit -a -u | grep processes | awk '{ print $5}')
user_process_count=$(ps -U niaaz | wc -l)
user_with_most_processes=$(ps -eo user|sort|uniq -c|sort -n -r|awk 'NR==1'| awk '{ print $2}')
mem_free_percentage=$(free | grep Mem | awk '{ print ($4 / $2) * 100}')
swap_free_percentage=$(free | grep Swap | awk '{ print ($4 / $2) * 100}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max User Processes: $max_user_process_count"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
echo "Swap Free (%): $swap_free_percentage"
