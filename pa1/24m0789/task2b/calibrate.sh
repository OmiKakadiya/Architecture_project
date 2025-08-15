#!/bin/bash

# Check if the user provided an input argument
if [ $# -eq 0 ]; then
  echo "Usage: ./calibrate.sh <input_for_program1>"
  exit 1
fi

# Capture the input argument
user_input="$1"

# Compile the two C programs
gcc -o program1 send.c
gcc -o program2 rec1.c

# Check if compilation was successful
if [ $? -ne 0 ]; then
  echo "Compilation failed. Please check your C code."
  exit 1
fi

# Run program2 on CPU 0
taskset -c 0 ./program2 &
program2_pid=$!

# Wait briefly to ensure program2 starts
sleep 1

# Run program1 on CPU 1 with the provided input
taskset -c 1 ./program1 "$user_input" &
program1_pid=$!

# Wait for program1 to finish
wait $program2_pid

# Terminate program2 to ensure it does not run indefinitely
kill $program1_pid 2>/dev/null

echo "Both programs have been terminated successfully."
