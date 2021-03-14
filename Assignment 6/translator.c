#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8000

int main()
{
    // printf("\nMaximum number of clients to connect: ");
    // scanf("%d", &MAX_CLIENTS);

    /* master_client_socket is used to connect to main socket running on server.
    master_server_client is used to connect to server.
    new_socket is used to create a new socket when a new client arrives.
    addrlen is used to store the length of addresses.
    client_sockets contains socket number of all connected clients.*/
    int master_client_socket, master_server_socket, addrlen, new_socket;
    int client_sockets[30] = {0};
    int client_ips[30];

    int opt = 1, activity, i, valread, sockid, max_sockid;
    struct sockaddr_in address_for_server, address_for_client; //sockaddr_in is a structure describing internet socket address.

    char buffer[1024] = {0}; // Maximum size of buffer. Buffer will save the received data from client. This is the maximum data that the server can receive.

    fd_set readfds; // descriptor set for select function.

    /* Creating socket file descriptor. socket creates a new socket of type UDP/TCP type in IPv4/ IPv6 domain, using protocol PROTOCOL.
	If PROTOCOL is zero, one is chosen automatically.
    For type: SOCK_STREAM: TCP(reliable, connection oriented), SOCK_DGRAM: UDP(unreliable, connectionless).
	Returns a file descriptor for the new socket, or -1 for errors. */
    printf("\nListening...");
    if ((master_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("\nClient Socket creation failed");
        exit(1);
    }
    if ((master_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nServer Socket creation failed");
        exit(1);
    }

    /* set options for socket. This is needed to reuse the socket and ip address for multiple connections. */
    if (setsockopt(master_client_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        printf("\nSet socket options failed");
        exit(1);
    }

    address_for_client.sin_family = AF_INET;
    address_for_client.sin_addr.s_addr = INADDR_ANY;
    address_for_client.sin_port = htons(PORT);
    memset(&address_for_server, 0, sizeof(address_for_server));
    address_for_server.sin_family = AF_INET;         // setting server socket to IPv4
    address_for_server.sin_addr.s_addr = INADDR_ANY; // to accept messages from localhost
    address_for_server.sin_port = htons(PORT);       // set port to one specified by user

    /* After creation of the socket, bind function binds the socket to the address and port number specified in addr(custom data structure).
	Returns -1 if any error occurs. */
    if (bind(master_client_socket, (struct sockaddr *)&address_for_client, sizeof(address_for_client)) < 0)
    {
        printf("\nBind failed");
        exit(1);
    }

    /* It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
    The backlog, defines the maximum length to which the queue of pending connections for sockfd may grow.
   If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED. */
    if (listen(master_client_socket, 3) < 0)
    {
        printf("\nListen failed");
        exit(1);
    }

    addrlen = sizeof(address_for_client);

    /* It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new connected socket,
   and returns a new file descriptor referring to that socket. At this point, connection is established between client and server, and they are ready to transfer data. */

    /* loop indefinately until the program is closed */
    while (1)
    {
        FD_ZERO(&readfds);                      // initialize readfds
        FD_SET(master_client_socket, &readfds); // set readfds to master_client_socket
        max_sockid = master_client_socket;      // max sockets

        for (i = 0; i < 30; i++)
        /* loop for all clients and set file descriptor bits for all connected sockets. If a new larger socketid is found, make it max_sockid */
        {
            sockid = client_sockets[i];
            if (sockid > 0)
                FD_SET(sockid, &readfds);
            if (sockid > max_sockid)
                max_sockid = sockid;
        }
        activity = select(max_sockid + 1, &readfds, NULL, NULL, NULL); //monitor multiple file descriptors set earlier

        if ((activity < 0) && (errno != EINTR))
        {
            printf("\nSelections failed");
        }

        if (FD_ISSET(master_client_socket, &readfds)) // check if file descriptor is set for master socket
        {
            if ((new_socket = accept(master_client_socket, (struct sockaddr *)&address_for_client, (socklen_t *)&addrlen)) < 0)
            {
                printf("\nAccept failed");
                exit(1);
            }
            /* if new connection is attempted*/

            for (i = 0; i < 30; i++)
            {
                /* save new client socket in first empty space in client socket list*/
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    printf("\nClient connected");
                    break;
                }
            }
        }
        for (i = 0; i < 30; i++)
        {
            sockid = client_sockets[i];

            // check all sockets for any message to be sent / received. FD_ISSET makes sure only clients conencted are checked.
            if (FD_ISSET(sockid, &readfds))
            {
                if ((valread = recv(sockid, buffer, 1024, MSG_DONTWAIT)) == 0)
                {
                    printf("\nClient disconnected");
                    close(sockid);
                    client_sockets[i] = 0;
                }
                else
                {
                    // read from client
                    buffer[valread] = '\0';
                    printf("\nMessage from client 192.168.0.%d: %s\n", sockid, buffer);
                    printf("Sending to server.....");
                    sendto(master_server_socket, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&address_for_server, sizeof(address_for_server));
                    printf("Done\n");
                    printf("Receiving from server.....");
                    recvfrom(master_server_socket, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&address_for_server, sizeof(address_for_server));
                    printf("Done\n");
                    printf("Sending to client.....");
                    send(sockid, buffer, strlen(buffer), MSG_DONTWAIT);
                    printf("Done\n");
                }
            }
        }
    }

    return 0;
}