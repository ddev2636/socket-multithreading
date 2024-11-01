#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <random>
#include <sstream>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

class UDPServer {
private:
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;
    atomic<int> clientCounter;  // Atomic counter for unique client IDs
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

public:
    UDPServer() : clientCounter(1), distribution(1, 10) {  // Random distribution for 1-10 words per send
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG SERVER] Socket created successfully." << endl;

        memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);

        if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("Bind failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG SERVER] Bind successful on port " << PORT << "." << endl;

        len = sizeof(sockaddr_in);
    }

    void serveClient(sockaddr_in cliaddr, const string &filename, int clientID) {
        string clientIp = inet_ntoa(cliaddr.sin_addr);

        ifstream file(filename);
        if (!file) {
            cerr << "[DEBUG SERVER] Error opening file for client " << clientID << " (IP: " << clientIp << ")" << endl;
            string errorMsg = "ERROR: File not found.";
            sendto(sockfd, errorMsg.c_str(), errorMsg.size(), 0, (const struct sockaddr *)&cliaddr, len);
            return;
        }

        cout << "[DEBUG SERVER] Serving file '" << filename << "' to client " << clientID << " (IP: " << clientIp << ")" << endl;

        // Read the file into memory as a list of words
        vector<string> words;
        string word;
        while (file >> word) {
            words.push_back(word);
        }
        file.close();

        // Send words in random groups until file end
        size_t wordIndex = 0;
        while (wordIndex < words.size()) {
            int numWordsToSend = distribution(generator);  // Random number of words to send
            stringstream message;

            // Collect random number of words for this turn
            for (int i = 0; i < numWordsToSend && wordIndex < words.size(); ++i) {
                message << words[wordIndex++] << " ";
            }

            // Send collected words to client
            string messageStr = message.str();
            sendto(sockfd, messageStr.c_str(), messageStr.size(), 0, (const struct sockaddr *)&cliaddr, len);
            cout << "[DEBUG SERVER] Sent to client " << clientID << " (IP: " << clientIp << "): " << messageStr << endl;

            //usleep(500000);  // Optional delay for demonstration
        }

        cout << "[DEBUG SERVER] File transfer complete for client " << clientID << " (IP: " << clientIp << ")" << endl;
    }

    void run() {
        vector<thread> clientThreads;

        while (true) {
            char buffer[BUFFER_SIZE];
            sockaddr_in cliaddr;
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
            buffer[n] = '\0';

            int clientID = clientCounter.fetch_add(1);  // Assign unique client ID
            string clientIp = inet_ntoa(cliaddr.sin_addr);
            cout << "[DEBUG SERVER] Received request from client " << clientID << " (IP: " << clientIp << "): '" << buffer << "'" << endl;

            string filename = string(buffer);

            // Start a new thread for each client
            clientThreads.emplace_back(&UDPServer::serveClient, this, cliaddr, filename, clientID);

            // Detach the thread to allow independent execution
            clientThreads.back().detach();
        }
    }
};

int main() {
    UDPServer server;
    server.run();
    return 0;
}
