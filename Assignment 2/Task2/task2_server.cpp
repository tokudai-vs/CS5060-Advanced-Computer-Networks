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
    int MAX_CLIENTS = 30;
    printf("\nMaximum number of clients to connect: ");
    scanf("%d", &MAX_CLIENTS);

    /* master_socket is used to connnect to main socket running on server.
    new_socket is used to create a new socket when a new client arrives.
    addrlen is used to store the length of addresses.
    client_sockets contains socket number of all connected clients.*/
    int master_socket, addrlen, new_socket, client_sockets[MAX_CLIENTS] = {0};

    int opt = 1, activity, i, valread, sockid, max_sockid;
    struct sockaddr_in address; //sockaddr_in is a structure describing internet socket address.

    char buffer[1024] = {0}; // Maximum size of buffer. Buffer will save the recived data from client. This is the maximum data that the server can recieve.

    fd_set readfds; // descriptor set for select function.

    /* Creating socket file descriptor. socket creates a new socket of type UDP/TCP type in IPv4/ IPv6 domain, using protocol PROTOCOL.
	If PROTOCOL is zero, one is chosen automatically.
   For type: SOCK_STREAM: TCP(reliable, connection oriented), SOCK_DGRAM: UDP(unreliable, connectionless). 
	Returns a file descriptor for the new socket, or -1 for errors. */
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("\nSocket creation failed");
        exit(1);
    }

    /* set options for socket. This is needed to reuse the socket and ip address for multiple connections. */
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        printf("\nSet socket options failed");
        exit(1);
    }

    address.sin_family = AF_INET;         // setting server socket to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // to accept messgaes from localhost
    address.sin_port = htons(PORT);       // set port to one specified by user
    printf("\nListening...");
    /* After creation of the socket, bind function binds the socket to the address and port number specified in addr(custom data structure).
	Returns -1 if any error occurs. */
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("\nBind failed");
        exit(1);
    }

    /* It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
   The backlog, defines the maximum length to which the queue of pending connections for sockfd may grow.
   If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED. */
    if (listen(master_socket, 3) < 0)
    {
        printf("\nListen failed");
        exit(1);
    }

    addrlen = sizeof(address);

    /* It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new connected socket,
   and returns a new file descriptor referring to that socket. At this point, connection is established between client and server, and they are ready to transfer data. */

    /* loop indefinately until the program is closed */
    while (1)
    {
        FD_ZERO(&readfds);               // intialize readfds
        FD_SET(master_socket, &readfds); // set readfds to master_socket
        max_sockid = master_socket;      // max sockets

        for (i = 0; i < MAX_CLIENTS; i++)
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

        if (FD_ISSET(master_socket, &readfds)) // check if file descriptor is set for master socket
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                printf("\nAccept failed");
                exit(1);
            }
            /* if new connection is attempted*/

            for (i = 0; i < MAX_CLIENTS; i++)
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
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sockid = client_sockets[i];

            // check all sockets for any message to be sent / recived. FD_ISSET makes sure only clients conencted are checked.
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
                    for (int k = 0; k < MAX_CLIENTS; k++)
                    {
                        int temp_sd = client_sockets[k];
                        // braodcast to all clients except from which recieved.
                        if (temp_sd != sockid)
                            send(temp_sd, buffer, strlen(buffer), MSG_DONTWAIT);
                    }
                }
            }
        }
    }

    return 0;
}