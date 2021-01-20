#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(void)
{
    // Craete server-side socket and report if failed.
    int sock_server = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_server < 0)
    {
        cout << "Socket creation failed for server.";
        return -1;
    }

    // Bind to port 8080
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    int bind_succ = bind(sock_server, (struct sockaddr *)&address, sizeof(address));
    if (bind_succ != 0)
    {

        cout << "Binding Server socket to port 8080 failed with error code " << bind_succ << " !" << endl;
        return -1;
    }

    // Listen on the port with backlog of 10
    int listen_succ = listen(sock_server, 10);
    if (listen < 0)
    {
        cout << "Listen not successfull" << endl;
        return -1;
    }

    // Create new connection and write back.
    int addrlen = sizeof(address);
    int socket_connect = accept(sock_server, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (socket_connect < 0)
    {
        cout << "Connection to client failed" << endl;
        return -1;
    }

    // Read from socket & print
    vector<char> buffer(1024);
    int readlen = read(socket_connect, &buffer[0], buffer.size());

    cout << "Read ";
    for (auto elem : buffer)
        cout << elem;
    cout << " from client." << endl;

    // Write to socket.
    string message = "Hello from server";
    send(socket_connect, message.c_str(), message.size(), 0);
}