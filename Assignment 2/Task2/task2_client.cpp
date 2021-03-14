#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

pthread_t threads[2];       // number of threads. One for sending, one for recieving.
char send_buffer[1000];     // buffer for sending data
char response_buffer[1000]; // buffer for recieving data
/* Socket descriptor contains the description of socket that is created.
	This includes detail such as port number, ip address, socket type (TCP / UDP), IPv4/ IPv6, etc */
int socket_descriptor;

/* function to recieve data. This function loops indefinately and prints data when some data from server arrives*/
void *readfromserver(void *threadargs)
{
   // int socket_descriptor = *((int *)threadargs);
   while (1)
   {
      int recvlen = recv(socket_descriptor, response_buffer, 1000, MSG_DONTWAIT);
      if (recvlen > 0)
      {
         /* print only when data is recieved */
         printf("\nR: %s\n", response_buffer);
      }
   }
}

/* function to send data to server. This function loops indefinately */
void *senttoserver(void *threadargs)
{
   // int socket_descriptor = *((int *)threadargs);
   while (1)
   {
      scanf("%s", send_buffer);
      // printf("S: %s\n", send_buffer);
      send(socket_descriptor, send_buffer, strlen(send_buffer), MSG_DONTWAIT);
   }
}

int main()
{
   int port = 8000; // Default port on which the server is listening
   int valread;

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

   /* It connects the socket referred to by the socket descriptor to the address specified by server. Server’s address and port is specified in server. */
   if (connect(socket_descriptor, (struct sockaddr *)&server, sizeof(server)) < 0)
   {
      printf("\nConnection Failed \n");
      close(socket_descriptor);
      exit(1);
   }

   printf("\nConnected to server\n");

   int thread_status;
   thread_status = pthread_create(&threads[0], NULL, &senttoserver, NULL);

   // printf("Thread created");
   if (thread_status)
   {
      printf("Send Thread creation failed");
   }

   thread_status = pthread_create(&threads[1], NULL, &readfromserver, NULL);
   if (thread_status)
   {
      printf("Receive Thread creation failed");
   }

   // close(socket_descriptor); //close the connection
   pthread_join(threads[0], NULL);
   pthread_join(threads[1], NULL);

   return 0;
}