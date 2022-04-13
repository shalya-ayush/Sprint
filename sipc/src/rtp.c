/**************************FILE HEADER********************************************* 
*    FILENAME       : rtp.c  
*
*    DESCRIPTION    :  It has function  to create rtp header and  
                       to send and recieve data from the server
*                                  
*                    
*     Revision History    :
*     DATE             NAME              REFERENCE                      REASON
*     -------------------------------------------------------------------------
*     07 April  2022    Group-6        SIP Protocol Implementation      New code 
*    
*     Copyright © 2022 
*
**********************************************************************************/

#include <header.h>
#include <rtp.h>
#include <sip.h>


void die_with_error(const char *message);

/************************************************************************************
*
*    FUNCTION NAME    :     mk_rtp_header(sip_session_t * seesion, int n)
*
*    DESCRIPTION      :    This function is used to make a rtp header
*
*    RETURN           :     char * pointer to the created rtp header
*
**************************************************************************************/
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

/************************************************************************************
*
*    FUNCTION NAME    :     rtp_recvfrom(int s, char * buf, int bufsize)
*
*    DESCRIPTION      :    This function is used to receive input from the server
*
*    RETURN           :     
*
**************************************************************************************/
int rtp_recvfrom(int s, char *buf, int bufsize)
{

    char tmpbuf[bufsize+RTP_HEADER_LEN];
    struct sockaddr_in from;
    int header[4], n, fromlen;

    fromlen = sizeof(from);

    if((n = recvfrom(s, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) &from, (socklen_t *) &fromlen)) != bufsize+RTP_HEADER_LEN){
        die_with_error("recvfrom() failed");
    }
        

    memcpy(header, tmpbuf, RTP_HEADER_LEN);
    memcpy(buf, tmpbuf+RTP_HEADER_LEN, n - RTP_HEADER_LEN);

    return n-RTP_HEADER_LEN;
}


/************************************************************************************
*
*    FUNCTION NAME    :     rtp_sendto(sip_session_t * session, char * buf, int bufsize,
                                     struct sockaddr_in * addr)
*
*    DESCRIPTION      :    It is used to send data to the server
*
*    RETURN           :     
*
**************************************************************************************/
int rtp_sendto(sip_session_t *session, char *buf, int bufsize, struct sockaddr_in *addr)
{

    int n;
    char tmpbuf[bufsize+RTP_HEADER_LEN];
    char *rtp_header;

    rtp_header = mk_rtp_header(session,bufsize);

    memcpy(tmpbuf, rtp_header, RTP_HEADER_LEN);
    memcpy(tmpbuf+RTP_HEADER_LEN, buf, bufsize);

    if((n = sendto(session->call->socket, tmpbuf, bufsize+RTP_HEADER_LEN, 0, (struct sockaddr *) addr, sizeof(struct sockaddr_in))) != bufsize+RTP_HEADER_LEN){
        die_with_error("sendto() failed");

    }


        

    

    return n-RTP_HEADER_LEN;

}
