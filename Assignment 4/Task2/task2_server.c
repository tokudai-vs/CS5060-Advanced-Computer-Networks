#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define PORT 8000
#define TIMEOUT 3
#define WINDOW_SIZE 5

int sockid, n;
struct sockaddr_in server;
int acknowledgements[50000] = {0};

pthread_t threads[1];

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

typedef struct thread_data
{
    int i;
    char data[BUFFER_SIZE];
} Thread_Data;

void *recvack(void *args)
{
    Ack acknowledge;
    n = recvfrom(sockid, &acknowledge, sizeof(acknowledge), 0, (struct sockaddr *)&server, sizeof(server));
    int ackfrom = acknowledge.acknum;
    acknowledgements[ackfrom] = 1;
}

int minack(int packets)
{
    int i = 0;
    for (i = 0; i < packets; i++)
    {
        if (acknowledgements[i] == 0)
        {
            return i;
        }
    }
    return -1;
}

void msleep(int tms)
{
    struct timeval tv;
    tv.tv_sec = tms / 1000;
    tv.tv_usec = (tms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
}

int main()
{
    int port = PORT;
    char buffer[BUFFER_SIZE];

    sockid = socket(AF_INET, SOCK_DGRAM, 0);
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
    n = recvfrom(sockid, &request, sizeof(request), 0, &server, sizeof(server));
    strcpy(buffer, request.payload.data);
    buffer[n] = '\0';

    FILE *f_pointer = fopen(buffer, "rb");
    if (f_pointer == NULL)
    {
        printf("Error in opening file.\n");
    }

    fseek(f_pointer, 0, SEEK_END);
    long f_size = ftell(f_pointer);
    fseek(f_pointer, 0, SEEK_SET);
    char *f_buffer = (char *)malloc((f_size + 1) * sizeof(char));

    InitialResponse response;
    response.packettype = 0;
    response.sequence_number = 0;
    response.file_size = f_size;
    response.window_size = WINDOW_SIZE;
    response.packetchecksum = 0;

    n = sendto(sockid, &response, sizeof(response), 0, (struct sockaddr *)&server, sizeof(server));
    if (n < 0)
    {
        printf("sendto failed.\n");
        exit(1);
    }
    Ack acknowledge;
    n = recvfrom(sockid, &acknowledge, sizeof(acknowledge), 0, (struct sockaddr *)&server, sizeof(server));
    if (n < 0)
    {
        printf("recvfrom failed.\n");
        exit(1);
    }
    long packets = ceil((double)response.file_size / (double)BUFFER_SIZE);

    char ack_array[BUFFER_SIZE];
    Payload f_data[packets];
    int count = 0;
    while (count < packets)
    {
        for (int i; i < f_size - 1; i++)
        {
            fread(f_buffer + i, 1, 1, f_pointer);
        }
        strcpy(f_data[count].data, f_buffer);
        count++;
    }

    int minpackettosend = 0;
    pthread_create(&threads[0], NULL, recvack, NULL);

    while (1)
    {
        if (minpackettosend == -1)
        {
            printf("Transfer completed");
            Ack finalpacket;
            finalpacket.packettype = 3;
            finalpacket.expected_sequence = 0;
            finalpacket.acknum = 0;
            finalpacket.packetchecksum = 0;

            n = sendto(sockid, &finalpacket, sizeof(finalpacket), 0, (struct sockaddr *)&server, sizeof(server));
            break;
        }

        for (int i = minpackettosend; i < minpackettosend + WINDOW_SIZE; i++)
        {
            Datapack packet;
            packet.packettype = 2;
            packet.sequence_number = i;
            packet.acknum = i;
            packet.packetchecksum = 0;
            packet.payloadsize = sizeof(f_data[i].data);
            strcpy(packet.payload.data, f_data[i].data);
            printf("Packet %d sent", i);

            n = sendto(sockid, &packet, sizeof(packet), 0, (struct sockaddr *)&server, sizeof(server));
        }
        msleep(TIMEOUT);
        minpackettosend = minack(packets);
    }
    close(sockid);
    close(f_pointer);
    pthread_join(threads[0], NULL);
    return 0;
}
