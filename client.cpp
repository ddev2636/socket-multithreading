#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "13.201.81.26"

class UDPClient
{
private:
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;

public:
    UDPClient()
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG CLIENT] Socket created successfully." << endl;

        memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        int res = inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);
        if (res <= 0)
        {
            perror("Invalid address/ Address not supported");
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG CLIENT] Server address set successfully." << endl;

        len = sizeof(servaddr);
    }

    void run()
    {
        string filename;
        cout << "Enter the file name: ";
        cin >> filename;

        filename.erase(remove_if(filename.begin(), filename.end(), [](unsigned char c)
                                 { return isspace(c); }),
                       filename.end());

        cout << "[DEBUG CLIENT] Cleaned filename: " << filename << endl;

        int bytesSent = sendto(sockfd, filename.c_str(), filename.size(), 0, (const struct sockaddr *)&servaddr, len);
        if (bytesSent < 0)
        {
            perror("Failed to send filename");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        cout << "[DEBUG CLIENT] Filename sent to server: " << filename << endl;

        char buffer[BUFFER_SIZE];
        int bytesReceived = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&servaddr, &len);
        if (bytesReceived < 0)
        {
            perror("Error receiving from server");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[bytesReceived] = '\0';
        cout << "[DEBUG CLIENT] Received response from server: " << buffer << endl;

        if (strcmp(buffer, "NOTFOUND") == 0)
        {
            cout << "File not found on server." << endl;
        }
        else
        {
            ofstream newFile("received_file.txt");
            newFile << buffer << endl;
            cout << "[DEBUG CLIENT] Received and saved first word: " << buffer << endl;

            while (true)
            {
                bytesReceived = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&servaddr, &len);
                if (bytesReceived < 0)
                {
                    perror("Error receiving word from server");
                    break;
                }
                buffer[bytesReceived] = '\0';

                if (strcmp(buffer, "END") == 0)
                {
                    cout << "[DEBUG CLIENT] 'END' received. Stopping file transfer." << endl;
                    break;
                }
                newFile << buffer << endl;
                cout << "[DEBUG CLIENT] Received and saved word: " << buffer << endl;
            }
            newFile.close();
        }

        close(sockfd);
    }
};

int main()
{
    UDPClient client;
    client.run();
    return 0;
}
