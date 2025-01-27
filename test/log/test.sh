#!/bin/bash
source ../common.sh

# run 10 times, thread_log use 1000 threads to log, expect 1001 lines
loop_run 10 "./thread_log | grep INFO | wc -l" 1001
INFO "thread_log test passed"

./log_file
# check if log_file.txt exists
if [ -f "log_file.txt" ]; then
    echo "log_file.txt exists"
else
    echo "log_file.txt does not exist"
    exit 1
fi
INFO "log_file test passed"

# check if log_file.txt contains "hello world"
if grep -q "hello world" log_file.txt; then
    echo "log_file.txt contains 'hello world'"
else
    echo "log_file.txt does not contain 'hello world'"
    exit 1
fi

has_debug=$(./log_level | grep -q "this is debug" | wc -l)
# should not have debug log
if [ $has_debug -eq 0 ]; then
    echo "no debug log"
else
    echo "has debug log"
    exit 1
fi
INFO "log_level test passed"
