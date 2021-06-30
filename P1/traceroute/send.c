/* 
 * Marcin
 * Dąbrowski
 * 315370
*/

#include "send.h"

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));

}

void prepareICMPHeader(struct icmp* header, int ttl, pid_t pid){
	header->icmp_type = ICMP_ECHO;
	header->icmp_code = 0;
	header->icmp_hun.ih_idseq.icd_id = pid;
	header->icmp_hun.ih_idseq.icd_seq = ttl;
	header->icmp_cksum = 0;
	header->icmp_cksum = compute_icmp_checksum(header, sizeof(*header));
  
}

int sendPackets(int sockfd, char *destinationAddr, int ttl, pid_t pid){ 
	struct sockaddr_in recipent;                               
	bzero(&recipent, sizeof(recipent));
	recipent.sin_family=AF_INET;
	inet_pton(AF_INET, destinationAddr, &recipent.sin_addr);

	struct icmp header;

  prepareICMPHeader(&header, ttl, pid);

	for (int i=0;i<3;i++){
		ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header), 0, (struct sockaddr*)&recipent, sizeof(recipent));
		if(bytes_sent == -1){
			fprintf(stderr, "An error occurred while sending packets!\n");
			return 1;
		}
	}
	return 0;
}
