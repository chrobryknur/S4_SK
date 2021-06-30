#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* returns 0 if all 3 packets were sent
   returns 1 if an error has occurred */
int sendPackets(int sockfd, char *destinationAddr, int ttl, pid_t pid);
