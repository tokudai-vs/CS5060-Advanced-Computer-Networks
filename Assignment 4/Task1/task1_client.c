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
#define FILE_NAME "5Mo.dat"
#define TIMEOUT 3000 //time is in microsecond | 1000 microsecond = 1 millisecond
typedef struct packet
{
    char data[BUFFER_SIZE];
} Packet;

typedef struct frame
{
    int type; //ACK:0, SEQ:1 FIN:2
    int seq_no;
    int ack;
    int checksum;
    int segment_size;
    Packet packet;
} Frame;

int main()
{
    int port = PORT;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server;

    int sockid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockid < 0)
    {
        printf("\nSocket creation failed \n");
        exit(1);
    }

    memset(&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    FILE *f_pointer = fopen(FILE_NAME, "rb");
    if (f_pointer == NULL)
    {
        printf("Error in opening file.\n");
        exit(1);
    }

    fseek(f_pointer, 0, SEEK_END);
    long f_size = ftell(f_pointer);
    fseek(f_pointer, 0, SEEK_SET);
    char *f_buffer = (char *)malloc((f_size + 1) * sizeof(char));

    for (int i; i < f_size - 1; i++)
    {
        fread(f_buffer+i, 1, 1, f_pointer);
    }
    int packets = ceil((double)f_size / (double)BUFFER_SIZE);

    sendto(sockid, &packets, sizeof(packets),0,(struct sockaddr *)&server,sizeof(server));
    printf("Sending %d packets\n", packets);
    int sent = BUFFER_SIZE;
    int i, remain = 0;
    int ack = 1, frame_id = 0;
    Frame send, recv;

    struct timeval start_time, current_time;
    while (packets > 0)
    {
        if (ack == 1)
        {
            send.seq_no = frame_id;
            send.type = 1;
            send.ack = 0;
            send.checksum = 0;
            send.segment_size = BUFFER_SIZE;

            for (i = 0; i < sent; i++)
            {
                buffer[i] = f_buffer[remain + i];
            }
            remain += i;
            if (f_size - remain < 1024)
            {
                sent = f_size - remain;
            }

            strcpy(send.packet.data, buffer);
            sendto(sockid, &send, sizeof(Frame), 0, (struct sockaddr *)&server, sizeof(server));
            gettimeofday(&start_time, NULL);
            printf("Frame %d sent\n", send.seq_no);
        }
        else
        {
            strcpy(send.packet.data, buffer);
            sendto(sockid, &send, sizeof(Frame), 0, (struct sockaddr *)&server, sizeof(server));
            gettimeofday(&start_time, NULL);
            printf("Frame %d sent\n", send.seq_no);
        }
        ack = 0;
        while (ack == 0)
        {
            gettimeofday(&current_time, NULL);
            if (current_time.tv_usec - start_time.tv_usec < TIMEOUT)
            {
                socklen_t server_size = sizeof(server);
                int n = recvfrom(sockid, &recv, sizeof(recv), 0, (struct sockaddr *)&server, &server_size);
                if (n > 0)
                {
                    printf("ACK Received\n");
                    ack = 1;
                    frame_id++;
                }
            }
            else
            {
                printf("Timeout for ACK\n");
                ack = 0;
                break;
            }
            packets--;
        }
    }
    close(sockid);
    return 0;
}
