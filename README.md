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
screen -S my_server_session
#Step 3: Run Your Server
./server
#Step 4: Detach from the Screen Session
Ctrl + A, then D
#Step 5: Reattach to the Screen Session
screen -r my_server_session
#Step 6: List Active Screen Sessions
screen -ls
#Step 7: Terminate a Screen Session
# If you need to terminate the screen session, reattach to it using screen -r my_server_session, press Ctrl + C to stop it. After that, type exit to close the screen session.
```

