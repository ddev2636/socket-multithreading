```bash
g++ -o client client.cpp
g++ -o server server.cpp
sed -i 's/\r$//' run_multiple_clients.sh
./run_multiple_clients.sh
```


```bash
#For continuosly running server
sudo apt-get update
#Step 1: Install screen
sudo apt-get install screen
#Step 2: Start a New Screen Session
screen -S server_session
#Step 3: Run Your Server
./server
#Step 4: Detach from the Screen Session
Ctrl + A, then D
#Step 5: Reattach to the Screen Session
screen -r server_session
#Step 6: List Active Screen Sessions
screen -ls
#Step 7: Terminate a Screen Session
# If you need to terminate the screen session, reattach to it using screen -r my_server_session, press Ctrl + C to stop it. After that, type exit to close the screen session.
```


```bash
#For updating server.cpp and recompiling and running server
sudo apt-get update
sudo apt-get install inotify-tools
```




```bash
##restart_server.sh##
#!/bin/bash

# Infinite loop to keep the script running
while true; do
    # Compile the server file
    g++ -o server server.cpp
    echo "[INFO] Server compiled successfully."

    # Run the server in the background
    ./server &
    SERVER_PID=$!

    # Monitor `server.cpp` for changes
    inotifywait -e close_write server.cpp

    # When a change is detected, kill the running server process
    echo "[INFO] Change detected in server.cpp. Restarting server..."
    kill $SERVER_PID
    wait $SERVER_PID 2>/dev/null
done
```
```bash
chmod +x restart_server.sh
screen -S server_session
./restart_server.sh
Ctrl A + then D
screen -r server_session
```




