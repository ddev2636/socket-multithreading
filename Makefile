# Makefile

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall

# Executables
CLIENT = client
SERVER = server

# Source files
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp

# Targets
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $(CLIENT) $(CLIENT_SRC)

$(SERVER): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER) received_file.txt
	echo "Cleaned up executables and received_file.txt"

.PHONY: all clean
