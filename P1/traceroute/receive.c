/* 
 * Marcin
 * Dąbrowski
 * 315370
*/

#include "receive.h"

struct response{
  uint8_t icmpType;
  uint16_t responseID;
  uint16_t responseSequenceNumber;
  char senderIP[20];
  double milisecondsSinceStart; 
};

double timeDifference(struct timeval start, struct timeval end){ // returns time difference in ms
  double secondsElapsed = end.tv_sec - start.tv_sec;
  double microsecondsElapsed = end.tv_usec - start.tv_usec;
  double result = secondsElapsed * 1000.0 + microsecondsElapsed/1000.0;
  return result;
}

int receiveResponse(int sockfd, char *ipAddr, int ttl, pid_t pid){ 
  fd_set descriptors;                                   
  FD_ZERO(&descriptors);                                
  FD_SET(sockfd, &descriptors);
  
  struct timeval start;
  struct timeval end;
  struct timeval timeForSelect; 

  gettimeofday(&start, 0);
  timeForSelect.tv_sec = 0;
  timeForSelect.tv_usec = 1000000;
  
  struct response allResponses[20];
  uint8_t index = 0; 

  struct sockaddr_in sender;
  socklen_t senderLength = sizeof(sender);
  u_int8_t buffer[IP_MAXPACKET];
  char senderIP[20];

  int receivedResponses = 0;

  while(receivedResponses < 3){
    int ready = select(sockfd+1, &descriptors, NULL, NULL, &timeForSelect);
    if(ready < 0){
      fprintf(stderr, "An error occurred with select()\n");
      return -1;
    } 
    if(ready == 0){ // 1 second elapsed
      break;
    }
     
    ssize_t packetLength = recvfrom(sockfd, buffer, IP_MAXPACKET,  0, (struct sockaddr*)&sender, &senderLength);
     
    if(packetLength <= 0){
      fprintf(stderr, "An error with recvfrom() occurred");
      return -1;
    }

		if(!inet_ntop(AF_INET, &(sender.sin_addr), senderIP, sizeof(senderIP))){
      fprintf(stderr, "An error with inet_ntop() occurred");
      return -1;
    }

    uint8_t icmpType = buffer[20];
    uint16_t packetId;
    uint16_t packetSequenceNumber;

    if(icmpType == ICMP_ECHOREPLY){
      packetId = buffer[25] << 8 | buffer[24];              // these bytes represent ICMP id field
      packetSequenceNumber = buffer[27] << 8 | buffer[26];  // these bytes represent ICMP seq field
    }  

    if(icmpType == ICMP_TIME_EXCEEDED){
      packetId = buffer[53] << 8 | buffer[52];
      packetSequenceNumber = buffer[55] << 8 | buffer[54];
    }

    if(packetId == pid && packetSequenceNumber == ttl){
      struct response temp;
      temp.icmpType = icmpType;
      gettimeofday(&end, 0);
      temp.milisecondsSinceStart = timeDifference(start, end);
      temp.responseID = packetId;
      temp.responseSequenceNumber  =  packetSequenceNumber;

      if(!memcpy(temp.senderIP, senderIP, 20)){
        fprintf(stderr, "An error with memcpy() occurred\n");
        return -1;
      }
      if(!memcpy(allResponses + index, &temp, sizeof(temp))){
        fprintf(stderr, "An error with memcpy() occurred\n");
        return -1;
      }

      index++;
      receivedResponses++;
    }
  }
  
  int finalDestinationReached = 0;
  double averageTime = 0.0;

  if(receivedResponses == 0){
    printf("%d. *\n", ttl);
    return 0;
  }

  else {
    printf("%d. ", ttl);
    for(int i=0;i<index;i++){
      averageTime += allResponses[i].milisecondsSinceStart;
      if(!strcmp(allResponses[i].senderIP, ipAddr)){
        finalDestinationReached = 1;
      }
      int sameAsPrevious = 1;
      for(int j=i-1; j>=0; j--){
        if(strcmp(allResponses[i].senderIP, allResponses[j].senderIP)){
          sameAsPrevious = 0;
        }
      }
      if(i == 0)
        sameAsPrevious = 0;
      if(!sameAsPrevious){
        printf("%s ", allResponses[i].senderIP);
      }
    }
  }

  if(receivedResponses == 3){
    printf("%dms\n", (int)(averageTime/3));
  }
  else{
    printf("???\n");
  }

  return finalDestinationReached;
}
