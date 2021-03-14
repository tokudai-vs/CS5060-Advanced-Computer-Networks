#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define DNS_SERVER_1 "192.168.35.52" // dns1.iith.ac.in
#define DNS_SERVER_2 "192.168.35.53"  // dns2.iith.ac.in
#define PORT 53                       // DNS port number

/* Reference: https://www2.cs.duke.edu/courses/fall16/compsci356/DNS/DNS-primer.pdf */
struct dns_header
{
    unsigned short id;        // 16 bit | DNS identifier
    unsigned char rd : 1;     // 1 bit | recursion desired
    unsigned char tc : 1;     // 1 bit | truncated message
    unsigned char aa : 1;     // 1 bit | authorative answer
    unsigned char opcode : 4; // 4 bit | kind of query
    unsigned char qr : 1;     // 1 bit | query(0) / response(1)
    unsigned char rcode : 4;  // 4 bit | response code
    unsigned char cd : 1;     // 1 bit | checking disabled
    unsigned char ad : 1;     // 1 bit | authentication data
    unsigned char z : 1;      // 1 bit | reserved
    unsigned char ra : 1;     // 1 bit | recursion available
    unsigned short qdcount;   // number of questions
    unsigned short ancount;   // number of answers
    unsigned short nscount;   // number of authority entries
    unsigned short arcount;   // number of resource entries
};

struct question
{
    unsigned short qtype;  // query type
    unsigned short qclass; // query class
};

struct response_data
{
    unsigned short type; // type
    unsigned short cls;
    unsigned int ttl;
    unsigned short datalen;
};

struct response
{
    unsigned char *name; // name
    struct response_data *resource;
    unsigned char *rdata;
};

void dnsformat(unsigned char *dnsname, unsigned char *hostname)
{
    int pos = 0, idx;
    strcat((char *)hostname, ".");
    int hostnamelen = strlen((char *)hostname);

    for (idx = 0; idx < hostnamelen; idx++)
    {
        if (hostname[idx] == '.')
        {
            *dnsname = idx - pos;
            *dnsname++;
            while (pos < idx)
            {
                *dnsname = hostname[pos];
                *dnsname++;
                pos++;
            }
            pos++;
        }
    }
    *dnsname = '\0';
    *dnsname++;
}

void gethost(unsigned char *hostaname, int query_type)
{
    int buffer_size = 60000; // buffer to store sending / recieving data
    unsigned char buffer[buffer_size], *quesname, *reader;

    struct sockaddr_in dnsserver, response;
    struct dns_header *dns = NULL;
    struct question *quesinfo = NULL;

    int sockid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    dnsserver.sin_family = AF_INET;
    dnsserver.sin_port = htons(PORT);
    dnsserver.sin_addr.s_addr = inet_addr(DNS_SERVER_1);

    dns = (struct dns_header *)&buffer;

    dns->id = (unsigned short)htons(getpid());
    dns->qr = 0;
    dns->opcode = 0;
    dns->aa = 0;
    dns->tc = 0;
    dns->rd = 1;
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->qdcount = htons(1);
    dns->ancount = 0;
    dns->nscount = 0;
    dns->arcount = 0;

    quesname = (unsigned char *)&buffer[sizeof(struct dns_header)];
    dnsformat(quesname, hostaname);
    quesinfo = (struct question *)&buffer[sizeof(struct dns_header) + (strlen((const char *)quesname) + 1)];

    quesinfo->qtype = htons(query_type);
    quesinfo->qclass = htons(1);

    printf("\nSending request...");
    int send_status = sendto(sockid, (char *)buffer, sizeof(struct dns_header) + (strlen((const char *)quesname) + 1) + sizeof(struct question), 0, (struct sockaddr *)&dnsserver, sizeof(dnsserver));
    if (send_status < 0)
    {
        printf("Sending failed\n");
    }
    else{
    printf("Done\n");
    }
    int dnsserver_size = sizeof(dnsserver);
    printf("\nRecieving response...");
    int recv_status = recvfrom(sockid, (char *)buffer, 65536, 0, (struct sockaddr *)&dnsserver, (socklen_t *)&dnsserver_size);

    if (recv_status < 0)
    {
        printf("Recieve failed\n");
    }
    else
    {
        printf("Done\n");
    }
    

    dns = (struct dns_header *)buffer;
    reader = &buffer[sizeof(struct dns_header) + (strlen((const char *)quesname) + 1) + sizeof(struct question)];

    int anscount = ntohs(dns->ancount);
    if (anscount > 1)
        printf("\nDNS response with mulitple answers is not parsed. Please retry with another hostanme.\n");

    struct response res[2];
    int stop = 2;

    res[0].resource = (struct response_data *)reader;
    reader += stop;
    res[0].resource = (struct response_data *)reader;
    reader += sizeof(struct response_data);
    res[0].rdata = (unsigned char *)malloc(ntohs(res[0].resource->datalen));

    for (int i = 0; i < ntohs(res[0].resource->datalen); i++)
        res[0].rdata[i] = reader[i];

    res[0].rdata[ntohs(res[0].resource->datalen)] = '\0';
    reader += ntohs(res[0].resource->datalen);

    long *data;
    data = (long *)res[0].rdata;
    response.sin_addr.s_addr = (*data);
    printf("\nIPv4 address: %s\n", inet_ntoa(response.sin_addr));
}

int main(int argc, char *argv[])
{
    unsigned char hostname[100];
    printf("Hostname to lookup : ");
    scanf("%s", hostname);
    gethost(hostname, 1);
    return 0;
}