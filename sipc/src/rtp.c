
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "../include/rtp.h"
#include "../include/sip.h"


void die_with_error(const char *message);

char *mk_rtp_header(sip_session_t *session, int n)
{
    char *rtp_header;

    int  timestamp;

    rtp_header = (char *) malloc(RTP_HEADER_LEN*sizeof(char));
    memset(rtp_header,0,RTP_HEADER_LEN);

    rtp_header[0] = 0x80;

    if(session->call->rtp_session.firstpkt == TRUE)
    {
        rtp_header[1] = 0x80;
        session->call->rtp_session.firstpkt = FALSE;
    }
    else
    {
        rtp_header[1] = 0x00;
    }

    rtp_header[3] = session->call->rtp_session.nseqpkt&0xFF;
    rtp_header[2] = (session->call->rtp_session.nseqpkt&0xFF00)>>8;

    timestamp = n*(session->call->rtp_session.nseqnon-1);
    rtp_header[7] = timestamp&0xFF;
    rtp_header[6] = (timestamp&0xFF00)>>8;
    rtp_header[5] = (timestamp&0xFF0000)>>16;
    rtp_header[4] = (timestamp&0xFF000000)>>24;

    /* SSRC */
    rtp_header[11] = SSRC&0xFF;

    return rtp_header;
}

int rtp_recvfrom(int s, char *buf, int bufsize)
{

    char tmpbuf[bufsize+RTP_HEADER_LEN];
    struct sockaddr_in from;
    int header[4], n, fromlen;

    fromlen = sizeof(from);

    if((n = recvfrom(s, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) &from, (socklen_t *) &fromlen)) != bufsize+RTP_HEADER_LEN)
        die_with_error("recvfrom() failed");

    memcpy(header, tmpbuf, RTP_HEADER_LEN);
    memcpy(buf, tmpbuf+RTP_HEADER_LEN, n - RTP_HEADER_LEN);

    return n-RTP_HEADER_LEN;
}

int rtp_sendto(sip_session_t *session, char *buf, int bufsize, struct sockaddr_in *addr)
{

    int n;
    char tmpbuf[bufsize+RTP_HEADER_LEN];
    char *rtp_header;

    rtp_header = mk_rtp_header(session,bufsize);

    memcpy(tmpbuf, rtp_header, RTP_HEADER_LEN);
    memcpy(tmpbuf+RTP_HEADER_LEN, buf, bufsize);

    if((n = sendto(session->call->socket, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) addr, sizeof(struct sockaddr_in))) != bufsize+RTP_HEADER_LEN)
        die_with_error("sendto() failed");

    // n = sendto(session->call->socket, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) addr, sizeof(struct sockaddr_in));
    // if(n < 0)
    //     die_with_error("sendto() failed");

    return n-RTP_HEADER_LEN;

}
