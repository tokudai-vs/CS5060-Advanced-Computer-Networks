#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

int main()
{ /* Socket descriptor contains the description of socket that is created.
	This includes detail such as port number, ip address, socket type (TCP / UDP), IPv4/ IPv6, etc */
   int socket_descriptor;
   char buffer[1024];
   int port = 8000; // Default port on which the server is listening
   int valread;
   struct timeval start_time, end_time; // time for calculating RTT

   struct sockaddr_in server; //sockaddr_in is a structure describing internet socket address.

   /* Creating socket file descriptor. socket creates a new socket of type UDP/TCP type in IPv4/ IPv6 domain, using protocol PROTOCOL.
	   If PROTOCOL is zero, one is chosen automatically.
      For type: SOCK_STREAM: TCP(reliable, connection oriented), SOCK_DGRAM: UDP(unreliable, connectionless).
	   Returns a file descriptor for the new socket, or -1 for errors. */
   if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      printf("\nSocket creation failed \n");
      exit(1);
   }

   // Filling server information
   server.sin_family = AF_INET;   // setting server socket to IPv4
   server.sin_port = htons(port); // set port to one specified by user

   // Convert IPv4 and IPv6 addresses from text to binary form. This is required
   if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
   {
      printf("\nInvalid address / Address not supported \n");
      exit(1);
   }

   /* It connects the socket referred to by the socket descriptor to the address specified by server. Server's address and port is specified in server. */
   if (connect(socket_descriptor, (struct sockaddr *)&server, sizeof(server)) < 0)
   {
      printf("\nConnection Failed \n");
      close(socket_descriptor);
      exit(1);
   }

   printf("\nConnected to server\n");
   while (1)
   {
      printf("\nMessage: ");
      scanf("%s", buffer);
      if(strcmp(buffer, "exit")==0){
         printf("Disconnecting\n");
         exit(0);
      }
      gettimeofday(&start_time, NULL);
      send(socket_descriptor, buffer, strlen(buffer), 0);
      int recvlen = recv(socket_descriptor, buffer, 1024, 0);
      buffer[recvlen] = '\0';
      gettimeofday(&end_time, NULL);

      printf("Reply: %s\n", buffer);
      printf("RTT: %ld\n", end_time.tv_usec - start_time.tv_usec);
   }

   return 0;
}