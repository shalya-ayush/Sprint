
// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "../include/sip.h"

#define PORT	 5060
#define MAXLINE 1024

// Driver code
int main() {
	char *r, *t, msg[MAX_SIP_LEN], c, buf[BUFSIZE];
    int n, p;


    struct sockaddr_in from;
    socklen_t fromlen;
    fromlen = sizeof(from);

    sip_session_t *session;
    session = (sip_session_t *) malloc(sizeof(struct sip_session_t));

    fd_set mask;

    
    session->pbxport = session->localport = -1;
    

    /* verify syntax */
    if (argc != 3);

    /* parse arguments */
    /*session->user */
    session->user = (char *) malloc((strlen(argv[1])+1)*sizeof(char));
    memcpy(session->user, argv[1], strlen(argv[1])+1);

    /*session->ext */
    session->ext = atoi(argv[2]);       

    /*session->pbxip*/
    r = strchr(argv[2], '@');
    if (r == NULL) usage();
    *r = '\0';
    session->pbxip = (char *) malloc((strlen(r)+1)*sizeof(char));
    memcpy(session->pbxip, r, strlen(r)+1);


    /*session ->pbxport */
    r++;
    t = strchr(r, ':');
    if (t == NULL) session->pbxport = DEFAULT_PORT;
    else *t = '\0';
    
    if (session->pbxport < 0)
    {
        t++;
        session->pbxport = atoi(t);
    }

    
    printf("Step-1: Arguments has beeen parsed Successfully..\n\n");

    
    session->addr.sin_family = AF_INET;
    session->addr.sin_addr.s_addr = inet_addr(session->pbxip);
    session->addr.sin_port = htons(session->pbxport);

	


}
