#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

// returns -1 in case of failure
// returns 0 if final destination was not reached
// returns 1 if final destination was reached
int receiveResponse(int sockfd, char *ipAddr, int ttl, pid_t pid);

