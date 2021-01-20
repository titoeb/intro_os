#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(void)
{
    // Craete client-side socket and report if failed.
    int sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_client < 0)
    {
        cout << "Socket creation failed for server.";
        return -1;
    }

    // Set adress for server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    int set_ip = inet_pton(AF_INET, "127.0.0.1", &server_address);
    if (set_ip < 0)
    {
        cout << "IP-Adress invalid!" << endl;
        return (-1);
    }

    // Connect to server
    int connect_server = connect(sock_client, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_server < 0)
    {
        cout << "Connection to server failed!" << endl;
        return (-1);
    }

    // Send to server and read response
    string message = "Hello from client";
    send(sock_client, message.c_str(), message.size(), 0);

    vector<char> buffer(1024);
    int readlen = read(sock_client, &buffer[0], buffer.size());

    cout << "Read ";
    for (auto elem : buffer)
        cout << elem;
    cout << " from server." << endl;
}