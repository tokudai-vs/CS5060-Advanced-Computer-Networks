#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8000
#define FILE_NAME "5Mo-received.dat"
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

int main(){

	int port = PORT;
    char buffer[BUFFER_SIZE];

	struct sockaddr_in server, new;

	int frame_id=0;
	Frame recv, send;

	int sockid = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&server, '\0', sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	bind(sockid, (struct sockaddr*)&server, sizeof(server));
	socklen_t addr_size = sizeof(new);

	FILE *f_pointer = fopen(FILE_NAME, "wb");
	if (f_pointer == NULL)
    {
        printf("Error in opening file.\n");
        exit(1);
    }
	int packets;
	recvfrom(sockid,&packets,sizeof(packets), 0, (struct sockaddr*)&server,sizeof(server));
	printf("Receiving %d packets\n", packets);

	while(packets > 0){
		if (packets == 0){
            send.seq_no = 0;
			send.type = 0;
			send.ack = recv.seq_no + 1;
            send.checksum = 0;
            send.segment_size =0;
            sendto(sockid, &send, sizeof(send), 0, (struct sockaddr*)&new, addr_size);
			printf("Ack Sent\n");
        }
        int n = recvfrom(sockid, &recv, sizeof(Frame), 0, (struct sockaddr*)&new, &addr_size);
		if (n > 0 && recv.type == 1){
			printf("Packet Received\n");
			fwrite(&recv.packet.data, sizeof(recv.packet.data),1,f_pointer);

			send.seq_no = 0;
			send.type = 0;
			send.ack = recv.seq_no + 1;
            send.checksum = 0;
            send.segment_size =0;
			sendto(sockid, &send, sizeof(send), 0, (struct sockaddr*)&new, addr_size);
			printf("Ack Send\n");
			// printf("\n sizeof(send) : %ld\n",sizeof(send));
		}else{
			printf("Frame Not Received\n");
		}
		frame_id++;
	packets--;
    }

	close(sockid);
	return 0;
}