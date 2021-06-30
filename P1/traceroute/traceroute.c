/* 
 * Marcin
 * Dąbrowski
 * 315370
*/

#include "send.h"
#include "receive.h"

#define  MAX_TTL 30

int properIpAddr(char* ipAddr, struct sockaddr_in* recipent){
	bzero(recipent, sizeof(*recipent));
	recipent->sin_family=AF_INET;
	return inet_pton(AF_INET, ipAddr, &recipent->sin_addr);
}

int main(int argc, char** argv)
{
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	struct sockaddr_in recipent;
	
	if(argc != 2 || !properIpAddr(argv[1], &recipent)){
		fprintf(stderr, "Usage: traceroute ip_addr");
		return EXIT_FAILURE;
	}

  pid_t pid = getpid()&0xFFFF;

	int done = 0;
	uint16_t ttl = 0;
	do{
		ttl++;

		if(setsockopt(sockfd, IPPROTO_IP, IP_TTL, (char*) &ttl, sizeof(ttl))){
			fprintf(stderr, "setsockopt() failed: %d\n", errno);
      return EXIT_FAILURE;
		}
		if(sendPackets(sockfd, argv[1], ttl, pid)){
      fprintf(stderr, "Sending packets failed\n");
      return EXIT_FAILURE;
    }
		done = receiveResponse(sockfd, argv[1], ttl, pid);
    if(done == -1){
      fprintf(stderr, "Receiving packets failed\n");
      return EXIT_FAILURE;
    }
	} while(!done && ttl<MAX_TTL);

	return EXIT_SUCCESS;
}
