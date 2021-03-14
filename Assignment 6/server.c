#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
	int packet_count = 0;
	char* nat_ip = "10.10.10.10";
	int port = 8000; // Default port on which the server will listen

	/* Socket descriptor contains the description of socket that is created.
	This includes detail such as port number, ip address, socket type (TCP / UDP), IPv4/ IPv6, etc */
	int socket_descriptor;

	char buffer[1024]; // Maximum size of buffer. Buffer will save the received data from client

	struct sockaddr_in server; //sockaddr_in is a structure describing internet socket address.

	/* Creating socket file descriptor. socket creates a new socket of type UDP/TCP type in IPv4/ IPv6 domain, using protocol PROTOCOL.
	If PROTOCOL is zero, one is chosen automatically.
   	For type: SOCK_STREAM: TCP(reliable, connection oriented), SOCK_DGRAM: UDP(unreliable, connectionless).
	Returns a file descriptor for the new socket, or -1 for errors. */
	if ((socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("Socket creation failed");
		exit(1);
	}

	memset(&server, 0, sizeof(server));

	// Filling server information
	server.sin_family = AF_INET;		 // setting server socket to IPv4
	server.sin_addr.s_addr = INADDR_ANY; // to accept messages from localhost
	server.sin_port = htons(port);		 // set port to one specified by user

	/* After creation of the socket, bind function binds the socket to the address and port number specified in addr(custom data structure).
	Returns -1 if any error occurs. */
	if (bind(socket_descriptor, (const struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("Bind failed");
		exit(1);
	}

	int len, n;

	len = sizeof(server); // length of server address
	printf("Listening... \n");
	while (1) // keep looping unless users enters bye
	{
		/* Receive data from the socket we bound to earlier. The data is stored in buffer which has a maximum size of 1024 bytes.
		It returns the size of buffer filled after receiving the data. */
		n = recvfrom(socket_descriptor, (char *)buffer, 1024,
					 MSG_WAITALL, (struct sockaddr *)&server,
					 &len);
		buffer[n] = '\0'; // terminate the received string

		printf("\nReceived from client %s:%d: %s\n", nat_ip, port, buffer); // print data received from client.

		/* Send the data back to client i.e. echo back data to client. sendto sends the data on the bound socket. */
		sprintf(buffer, "Received packet %d from %s:%d", packet_count, nat_ip, port);
		packet_count++;
		sendto(socket_descriptor, (const char *)buffer, strlen(buffer),
			   MSG_CONFIRM, (const struct sockaddr *)&server,
			   len);
	}
	return 0;
}
