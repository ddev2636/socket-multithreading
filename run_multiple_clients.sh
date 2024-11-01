#!/bin/bash

# Number of clients to run
num_clients=3
# Array of files to request
filenames=("demo_file.txt" "demo_file.txt" "demo_file.txt")

# Loop to start multiple clients
for ((i=0; i<num_clients; i++)); do
    echo "Starting client $((i + 1)) to request ${filenames[i]}..."
    # Run the client and provide the corresponding filename as input
    echo "${filenames[i]}" | ./client &
done

# Wait for all clients to finish
wait
echo "All clients have completed."
