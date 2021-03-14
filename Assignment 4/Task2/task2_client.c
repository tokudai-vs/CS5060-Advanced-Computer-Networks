#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define PORT 8000
#define TIMEOUT 3000
// #define FILE_NAME "5Mo.dat"

typedef struct payload
{
    char data[BUFFER_SIZE];
} Payload;

typedef struct initialrequest
{
    int packettype; // 0: SYN, 1 :ACK, 2: SEQ, 3: FIN
    int sequence_number;
    int acknum;
    int packetchecksum;
    Payload payload;
} InitialRequest;

typedef struct intialresponse
{
    int packettype; // 0: SYN, 1 :ACK, 2: SEQ, 3: FIN
    int sequence_number;
    int file_size;
    int window_size;
    int packetchecksum;
} InitialResponse;

typedef struct datapack
{
    int packettype; // 0: SYN, 1 :ACK, 2: SEQ, 3: FIN
    int sequence_number;
    int acknum;
    int packetchecksum;
    int payloadsize;
    Payload payload;
} Datapack;

typedef struct ack
{
    int packettype; // 0: SYN, 1 :ACK, 2: SEQ, 3: FIN
    int expected_sequence;
    int acknum;
    int packetchecksum;
} Ack;

int main()
{
    char FILE_NAME[50];
    printf("File to be transferred: ");
    scanf("%s", FILE_NAME);
    int port = PORT;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server;
    int n;

    int sockid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockid < 0)
    {
        printf("Socket creation failed.\n");
        exit(1);
    }
    else
    {
        printf("Socket created successfully.\n");
    }

    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    InitialRequest request;
    request.packettype = 0;
    request.sequence_number = 0;
    request.acknum = 0;
    request.packetchecksum = 0;
    strcpy(request.payload.data, FILE_NAME);
    n = sendto(sockid, &request, sizeof(request), 0, (struct sockaddr *)&server, sizeof(server));
    if (n < 0)
    {
        printf("sendto failed.\n");
        exit(1);
    }
    InitialResponse response;
    n = recvfrom(sockid, &response, sizeof(response), 0, (struct sockaddr *)&server, sizeof(server));
    if (n < 0)
    {
        printf("recvfrom failed.\n");
        exit(1);
    }
    Ack acknowledge;
    acknowledge.packettype = 1;
    acknowledge.expected_sequence = 0;
    acknowledge.acknum = 0;
    acknowledge.packetchecksum = 0;
    n = sendto(sockid, &acknowledge, sizeof(acknowledge), 0, (struct sockaddr *)&server, sizeof(server));
    if (n < 0)
    {
        printf("sendto failed.\n");
        exit(1);
    }
    long packets = ceil((double)response.file_size / (double)BUFFER_SIZE);

    char data_array[packets];

    while (1)
    {
        Datapack packet;
        n = recvfrom(sockid, &packet, sizeof(packet), 0, (struct sockaddr *)&server, sizeof(server));
        if (n < 0)
        {
            printf("recvfrom failed.\n");
            exit(1);
        }
        else
        {
            printf("Packet received \n");
        }
        if (packet.packettype == 3)
        {
            printf("Transmission complete.\n");
            break;
        }
        acknowledge.packettype = 1;
        acknowledge.expected_sequence = packet.sequence_number + 1;
        acknowledge.acknum = packet.sequence_number;
        acknowledge.packetchecksum = 0;
        n = sendto(sockid, &acknowledge, sizeof(acknowledge), 0, (struct sockaddr *)&server, sizeof(server));
        if (n < 0)
        {
            printf("sendto failed.\n");
            exit(1);
        }
        else
        {
            printf("Acknowledgement sent \n");
        }
    }
    close(sockid);
    return 0;
}
