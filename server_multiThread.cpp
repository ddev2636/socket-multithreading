#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <algorithm>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

class UDPServer
{
private:
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

public:
    UDPServer()
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG SERVER] Socket created successfully." << endl;

        memset(&servaddr, 0, sizeof(servaddr));
        memset(&cliaddr, 0, sizeof(cliaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);

        if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("Bind failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG SERVER] Bind successful on port " << PORT << "." << endl;

        len = sizeof(cliaddr);
    }

    void handleClientRequest(string filename)
    {
        cout << "[DEBUG SERVER] Handling request for file: " << filename << endl;
        ifstream file(filename);
        if (!file)
        {
            string notFound = "NOTFOUND";
            sendto(sockfd, notFound.c_str(), notFound.size(), 0, (const struct sockaddr *)&cliaddr, len);
            cerr << "[DEBUG SERVER] File not found: " << filename << endl;
            return;
        }
        cout << "[DEBUG SERVER] File opened successfully: " << filename << endl;

        string word;
        file >> word;
        sendto(sockfd, word.c_str(), word.size(), 0, (const struct sockaddr *)&cliaddr, len);
        cout << "[DEBUG SERVER] Sent to client: " << word << endl;

        while (file >> word)
        {
            if (word == "END")
            {
                sendto(sockfd, word.c_str(), word.size(), 0, (const struct sockaddr *)&cliaddr, len);
                cout << "[DEBUG SERVER] Sent to client: " << word << " (End of file)" << endl;
                break;
            }

            sendto(sockfd, word.c_str(), word.size(), 0, (const struct sockaddr *)&cliaddr, len);
            cout << "[DEBUG SERVER] Sent to client: " << word << endl;
        }
        cout << "[DEBUG SERVER] File transfer complete." << endl;
        file.close();
    }

    void run()
    {
        while (true)
        {
            char buffer[BUFFER_SIZE];
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
            buffer[n] = '\0';
            cout << "[DEBUG SERVER] Received request from client: '" << buffer << "'" << endl;

            string filename = string(buffer);
            cout << "[DEBUG SERVER] Spawned new thread for request." << endl;

            thread clientThread(&UDPServer::handleClientRequest, this, filename);
            clientThread.detach();
        }
    }
};

int main()
{
    std::cout << "C++ Version: " << __cplusplus << std::endl;
    UDPServer server;
    server.run();
    return 0;
}
