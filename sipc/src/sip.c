/**************************FILE HEADER********************************************* 
*    FILENAME       : sip.c  
*
*    DESCRIPTION    :  All the functions related to sip protocol are included here  
*                                  
*                    
*     Revision History    :
*     DATE             NAME              REFERENCE                      REASON
*     -------------------------------------------------------------------------
*     09 April  2022    Group-6        SIP Protocol Implementation      New code 
*    
*     Copyright © 2022 
*
**********************************************************************************/

#include <header.h>
#include <sip.h>

int open_udp_socket(char *ip, int port);
void die_with_error(const char *message);
extern int textfd;

/************************************************************************************
*
*    FUNCTION NAME    :     mk_sip_msg(sip_session_t * session, int exp, int msgtype)
*
*    DESCRIPTION      :    It is used to make different SIP messages according to the SIP format
*
*    RETURN           :     char * to the created message
*
**************************************************************************************/


char *mk_sip_msg(sip_session_t *session, int exp, int msgtype)
{
    
    char *msg = NULL;
    char sdp[MAX_SDP_LEN];

    msg = (char *) malloc(MAX_SIP_LEN * sizeof(char));

    switch(msgtype)
    {
    case REGISTER:
        snprintf(msg, MAX_SIP_LEN, \
                 "REGISTER sip:%s:%i SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%i;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "To: %s <sip:%i@%s:%i>\n"\
                 "From: %s <sip:%i@%s:%i>;tag=456248\n"\
                 "Call-ID: a2e3a@%s\n"\
                 "CSeq: %d REGISTER\n"\
                 "Contact: %s <sip:%i@%s:%i>\n"\
                 "Expires: %i\n"\
                 "Content-Length: 0\n\n\n",\
                 session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->localip,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport,\
                 exp);

        break;
    case INVITE:
        snprintf(sdp, sizeof(sdp), \
                 "c=IN IP4 %s\n"\
                 "m=text %d RTP/AVP 0\n", session->localip, session->call->sport);

        snprintf(msg, MAX_SIP_LEN, \
                 "INVITE sip:%s@%s:%i SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%i;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "To: sip:%s@%s:%d\n"\
                 "From: %s <sip:%d@%s:%d>;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d INVITE\n"\
                 "Contact: %s <sip:%d@%s:%d>\n"\
                 "Content-Type: application/sdp\n"\
                 "Content-Length: %ld\n\n"\
                 "%s",\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport,\
                 strlen(sdp),\
                 sdp);
        break;
    case INVITE_OK:
        snprintf(sdp, MAX_SDP_LEN, \
                 "c=IN IP4 %s\n"\
                 "m=text %d RTP/AVP 0\n", session->localip, session->call->sport);

        snprintf(msg, MAX_SIP_LEN, \
                 "SIP/2.0 200 OK\n"\
                 "Via: SIP/2.0/UDP %s:%i;branch=z9hG4bK\n"\
                 "To: sip:%d@%s:%d;tag=%s\n"\
                 "From: %s\" <sip:%s@%s:%d>;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d INVITE\n"\
                 "Contact: %s <sip:%d@%s:%d>\n"\
                 "Content-Type: application/sdp\n"\
                 "Content-Length: %ld\n"\
                 "\n"\
                 "%s",
                 session->call->dip, session->call->dsport,\
                 session->ext, session->pbxip, session->pbxport, session->call->tag,\
                 session->call->duser, session->call->dext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport,\
                 strlen(sdp),\
                 sdp);
        break;
    case ACK:
        snprintf(msg, MAX_SIP_LEN, \
                 "ACK sip:%s@%s:%d SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "To: sip:%s@%s:%d;tag=%s\n"\
                 "From: %s <sip:%i@%s:%d>;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d ACK\n"\
                 "Contact: %s <sip:%i@%s:%d>\n"\
                 "Content-Length: 0\n\n\n",\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->call->dext, session->pbxip, session->pbxport, session->call->tag,\
                 session->user, session->ext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq,\
                 session->user, session->ext, session->localip, session->localport);
        break;
    case BYE:
        snprintf(msg, MAX_SIP_LEN, \
                 "BYE sip:%s@%s:%d SIP/2.0\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "Max-Forwards: 70\n"\
                 "From: sip:%i@%s:%d;tag=%s\n"\
                 "To: sip:%s@%s:%d;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d BYE\n"\
                 "Content-Length: 0\n\n\n",
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->localip, session->localport,\
                 session->ext, session->pbxip, session->pbxport, session->call->tag,\
                 session->call->dext, session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq);
        break;
    case BYE_OK:
        snprintf(msg, MAX_SIP_LEN, \
                 "SIP/2.0 200 OK\n"\
                 "Via: SIP/2.0/UDP %s:%d;branch=z9hG4bK\n"\
                 "From: sip:%s@%s:%d;tag=%s\n"\
                 "To: sip:%i@%s:%d;tag=19283\n"\
                 "Call-ID: %s\n"\
                 "CSeq: %d BYE\n"\
                 "Content-Length: 0\n\n\n",
                 session->call->dip, session->call->dport,\
                 session->call->dext, session->pbxip, session->pbxport, session->call->tag,\
                 session->ext,  session->pbxip, session->pbxport,\
                 session->call->id,\
                 session->seq);
        break;
    }

    
    return msg;
}


/************************************************************************************
*
*    FUNCTION NAME    :    read_sip_msg(char * str) 
*
*    DESCRIPTION      :    This function is used for reading the SIP messages and assigning value to 
                            all the fields of sip_msg_t
*
*    RETURN           :     sip_msg_t * 
*
**************************************************************************************/

sip_msg_t *read_sip_msg(char *str)
{
    char **s = NULL;
    char *temp_str = NULL;
    char *r = NULL;
    char n[3][100];

    int i = INIT;
    int k = INIT;
    int p=  INIT;

    sip_msg_t *msg = NULL;

    s = (char **) malloc(MAX_SIP_LINES * sizeof(char*));
    temp_str = str;

    for(i=0; (r = strchr(temp_str, '\n')) != NULL;i++)
    {
        *r = '\0';
        s[i] = (char *) malloc((strlen(temp_str)+1) * sizeof(char));
        strcpy(s[i], temp_str);
        *r = '\n';
        temp_str = r+1;
    }

    s[i]=NULL;

    msg = (sip_msg_t *) malloc(sizeof(sip_msg_t));
    msg->call = (sip_call_t *) malloc(sizeof(sip_call_t));
    msg->call->dip = NULL;
    msg->call->dext = NULL;
    msg->call->duser = NULL;
    msg->call->tag = NULL;
    msg->call->id = NULL;
    msg->method = NULL;
    msg->ip = NULL;
    msg->version = NULL;
    msg->via = NULL;
    msg->maxforwards = NULL;
    msg->to = NULL;
    msg->from = NULL;
    msg->callid = NULL;
    msg->contact = NULL;
    msg->contenttype = NULL;

    printf("%s\n",s[0]);

    sscanf(s[0], "%s %s %s", n[0], n[1], n[2]);

    if(strstr(n[0], "SIP"))
    {
        msg->type = ANSWER;
        msg->code = atoi(n[1]);
    }
    else if(strstr(n[2], "SIP"))
    {
        msg->type = REQUEST;
        msg->method = (char *) malloc(strlen(n[0])*sizeof(char));
        memcpy(msg->method,n[0],strlen(n[0]));
    }

    for(k=0;k<i;k++)
    {

        temp_str = strchr(s[k],':');
        if(temp_str != NULL)
        {
            *temp_str = '\0';

            if(!strcmp(s[k],"Via"))
            {
                msg->via = (char *) malloc(strlen(temp_str+2)*sizeof(char));
                memcpy(msg->via,temp_str+2,strlen(temp_str+2));
            }
            else if(!strcmp(s[k],"To"))
            {
                msg->to = (char *) malloc(strlen(temp_str+2)*sizeof(char));
                memcpy(msg->to,temp_str+2,strlen(temp_str+2));
                r = strchr(temp_str+2,'=');
                if(r != NULL)
                {
                    r++;
                    msg->call->tag = (char *) malloc((strlen(r)+1)*sizeof(char));
                    memset(msg->call->tag,'\0',strlen(r)+1);
                    memcpy(msg->call->tag,r,strlen(r)+1);
                }
            }
            else if(!strcmp(s[k],"From"))
            {
                msg->from = (char *) malloc(strlen(temp_str+2)*sizeof(char));
                memcpy(msg->from,temp_str+2,strlen(temp_str+2));

                if((r = strchr(temp_str+2,' ')) != NULL)
                {
                    *r = '\0';
                    msg->call->duser = (char *) malloc((strlen(temp_str+2)+1)*sizeof(char));
                    memset(msg->call->duser,'\0',strlen(temp_str+2)+1);
                    memcpy(msg->call->duser,temp_str+2,strlen(temp_str+2)+1);
                }
                else if((r = strchr(temp_str+2,'"')) != NULL)
                {

                    *r = '\0';
                    r = strchr(temp_str+3,'"');
                    *r = '\0';
                    msg->call->duser = (char *) malloc((strlen(temp_str+3)+1)*sizeof(char));
                    memset(msg->call->duser,'\0',strlen(temp_str+3)+1);
                    memcpy(msg->call->duser,temp_str+3,strlen(temp_str+3)+1);

                }
            }
            else if(!strcmp(s[k],"Call-ID"))
            {
                msg->callid = (char *) malloc((strlen(temp_str+2)+1)*sizeof(char));
                memset(msg->callid,'\0',strlen(temp_str+2)+1);
                memcpy(msg->callid,temp_str+2,strlen(temp_str+2)+1);

                msg->call->id = (char *) malloc((strlen(temp_str+2)+1)*sizeof(char));
                memset(msg->call->id,'\0',strlen(temp_str+2)+1);
                memcpy(msg->call->id,temp_str+2,strlen(temp_str+2)+1);
            }
            else if(!strcmp(s[k],"CSeq"))
            {
                msg->cseq = atoi(temp_str+2);
            }
            else if(!strcmp(s[k],"Contact"))
            {
                msg->contact = (char *) malloc((strlen(temp_str+2)+1)*sizeof(char));
                memset(msg->contact,'\0',strlen(temp_str+2)+1);
                memcpy(msg->contact,temp_str+2,strlen(temp_str+2)+1);
                r = strchr(temp_str+2,':');
                r++;
                temp_str =strchr(r,'@');
                *temp_str = '\0';
                msg->call->dext = (char *) malloc((strlen(r)+1)*sizeof(char));
                memset(msg->call->dext,'\0',strlen(r)+1);
                memcpy(msg->call->dext,r,strlen(r)+1);
                temp_str++;
                r = strchr(temp_str,':');
                if(r == NULL)
                {
                    msg->call->dsport = DEFAULT_PORT;
                    r = strchr(temp_str,'>');
                }
                *r = '\0';
                msg->call->dip = (char *) malloc((strlen(temp_str)+1)*sizeof(char));
                memset(msg->call->dip,'\0',strlen(r)+1);
                memcpy(msg->call->dip,temp_str,strlen(temp_str)+1);
                if(msg->call->dsport != DEFAULT_PORT)
                {
                    r++;
                    temp_str = strchr(r,'>');
                    *temp_str = '\0';
                    msg->call->dsport = atoi(r);
                }

            }
            else if(!strcmp(s[k],"Content-Type"))
            {

                r = strchr(temp_str+2,'/');
                *r = '\0';
                if(!strcmp(temp_str+2,"application"))
                {

                    p = 1;
                }

            }
            else if(!strcmp(s[k],"Content-Length"))
            {
                msg->contentlength = atoi(temp_str+2);

            }
            else if(!strcmp(s[k],"Max-Forwards"))
            {
                msg->maxforwards = (char *) malloc(strlen(temp_str+2)*sizeof(char));
                memcpy(msg->maxforwards,temp_str+2,strlen(temp_str+2));
            }
            else if(!strcmp(s[k],"Expires"))
            {
                msg->expires = atoi(temp_str+2);
            }
        }


        if(p==1)
        {

            r = strchr(s[k],' ');
            if (r == NULL) continue;
            // *r = '\0';
            printf("s[k]=\n%s\n",s[k]);

            if(!strcmp(s[k],"m=text"))
            {
                msg->call->dport = atoi(r+1);
            }
        }
    }

    for(k=0;k<i+1;k++){
        free(s[k]);
    }
    
    free(s);

    return msg;

}

/************************************************************************************
*
*    FUNCTION NAME    :     gen_call_id(sip_session_t * session)
*
*    DESCRIPTION      :    It is used to generate a random call id for session->call->id field
*
*    RETURN           :     char * to the generated call id
*
**************************************************************************************/
char *gen_call_id(sip_session_t *session)
{
    char *s;

    s = (char *) malloc(10*sizeof(char));
    snprintf(s, 9, "%i", rand());

    return s;
}

/************************************************************************************
*
*    FUNCTION NAME    :     send_sip_register(sip_session_t *session, int exp)
*
*    DESCRIPTION      :    This function is used to send the REGISTER packet to the server
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void send_sip_register(sip_session_t *session, int exp)
{
    
    int n = INIT;
    char *msg = NULL;

    msg = mk_sip_msg(session, exp, REGISTER);
    
    
    if ((n = sendto(session->socket, msg, strlen(msg) + 1, 0, (struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(msg) + 1)
        die_with_error("sendto() failed");

    free(msg);


}

/************************************************************************************
*
*    FUNCTION NAME    :     init_sip_session(sip_session_t * session)
*
*    DESCRIPTION      :    This function is used to Initialize a SIP session
*
*    RETURN           :    Nothing 
*
**************************************************************************************/
void init_sip_session(sip_session_t *session)
{

    
    session->call = NULL;
    session->seq = 1;
    session->curr_state = REGISTERING;
    session->prev_state = INIT;

    session->socket = open_udp_socket(session->localip, session->localport);
    send_sip_register(session, DEFAULT_EXPIRATION);


}

/************************************************************************************
*
*    FUNCTION NAME    :     send_sip_invite(sip_session_t * session)
*
*    DESCRIPTION      :    This function is used to send the INVITE packet to the sever
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void send_sip_invite(sip_session_t *session)


{
    
    int len = INIT;
    char *s = NULL;
    struct sockaddr_in addr;

    s = gen_call_id(session);

    session->call->id = (char *) malloc(strlen(s)* sizeof(char));
    memcpy(session->call->id,s,strlen(s));

    session->call->socket = open_udp_socket(session->localip,0);

    len = sizeof(addr);
    if(getsockname(session->call->socket,(struct sockaddr *) &addr,(socklen_t *) &len)){

        die_with_error("getsockname() failed");
    }

    session->call->sport = ntohs(addr.sin_port);
     
    s = mk_sip_msg(session,0,INVITE);
    

    len =sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr));
    

    if(len < 0){
        die_with_error("sendto() failed");
    }
    

    
    session->prev_state = REGISTERED;
    session->curr_state = INVITING;
    

}

/************************************************************************************
*
*    FUNCTION NAME    :     send_sip_bye(sip_session_t * session)
*
*    DESCRIPTION      :    This function is used to send the BYE packet
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void send_sip_bye(sip_session_t *session)
{
    int n = INIT;
    char *msg = NULL;

    msg = mk_sip_msg(session,0,BYE);

    if((n =sendto(session->socket,msg,strlen(msg)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr)) ) != strlen(msg)+1)
    {
        die_with_error("sendto() failed");
    }

    close(session->call->socket);

    session->prev_state = ONCALL;
    session->curr_state = BYE;
}

/************************************************************************************
*
*    FUNCTION NAME    :     close_sip_session(sip_session_t * session)
*
*    DESCRIPTION      :    This function is used to close the SIP session
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void close_sip_session(sip_session_t *session)
{
    send_sip_register(session, 0);

    session->prev_state = REGISTERED;
    session->curr_state = UNREGISTERING;
}

/************************************************************************************
*
*    FUNCTION NAME    :     free_sip_msg(sip_msg_t * msg)
*
*    DESCRIPTION      :    This fucntion is used to free the created SIP message
*
*    RETURN           :    Nothing 
*
**************************************************************************************/
void free_sip_msg(sip_msg_t *msg)
{

    if(msg->method != NULL){
        free(msg->method);
    }
    if(msg->ip != NULL) {
        free(msg->ip);
    }
    if(msg->maxforwards != NULL) {
        free(msg->maxforwards);
    }
    if(msg->to != NULL) {
        free(msg->to);
    }
    if(msg->from != NULL) {
        free(msg->from);
    }
    if(msg->callid != NULL) {
        free(msg->callid);
    }
    if(msg->contact != NULL) {
        free(msg->contact);
    }
    if(msg->contenttype != NULL) {
        free(msg->contenttype);
    }

    free(msg);

}

/************************************************************************************
*
*    FUNCTION NAME    :     free_sip_call(sip_call_t * call)
*
*    DESCRIPTION      :    This function is used to free the SIP session call
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void free_sip_call(sip_call_t *call)
{
    if(call->dext != NULL) {
        free(call->dext);
    } 
    if(call->dip != NULL) {
        free(call->dip);
    } 
    if(call->duser != NULL) {
        free(call->duser);
    } 
    if(call->tag != NULL) {
        free(call->tag);
    } 
    if(call->id != NULL) {
        free(call->id);
    } 

    free(call);
}

/************************************************************************************
*
*    FUNCTION NAME    :     free_sip_session(sip_session_t * session)
*
*    DESCRIPTION      :    This function is used to free the memory assigned during the SIP session
                           creation
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void free_sip_session(sip_session_t *session)
{

    if(session->pbxip != NULL) {
        free(session->pbxip);
    } 
    if(session->localip != NULL) {
        free(session->localip);
    } 
    if(session->user != NULL) {
        free(session->user);
    } 

    free(session);
}

/************************************************************************************
*
*    FUNCTION NAME    :     handle_sip_msg(sip_session_t * session, char * str)
*
*    DESCRIPTION      :    This function is used to handle the sip message and to change the state of the 
                           program after each packet transfer.
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void handle_sip_msg(sip_session_t *session, char *str)
{

    int l;
    char *s = NULL;
    sip_msg_t *msg = NULL;
    printf("str = %s", str);
    struct sockaddr_in addr;

    

    msg = read_sip_msg(str);
    
    
    switch(session->curr_state)
    {
    case REGISTERING:
        printf("#### Case = REGISTERING ####\n");
        // if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        if(msg->type == REQUEST)
        {
            fprintf(stdout,"Register executed successfully.\n");
            session->prev_state = REGISTERING;
            session->curr_state = REGISTERED;
        }
        else if(msg->type == ANSWER && msg->code < 100 && msg->code > 200)
        {
            fprintf(stdout,"Register not executed.\n");
            exit(EXIT_FAILURE);
        }
        free_sip_call(msg->call);
        break;
    case REGISTERED:
        printf("#### Case = REGISTERED ####\n");
        if(msg->type == REQUEST && !strcmp(msg->method,"INVITE") && msg->contact != NULL)
        {
            session->call = msg->call;
            session->seq = msg->cseq;

            session->call->sport = ntohs(addr.sin_port);


            session->call->addr.sin_family = AF_INET;
            session->call->addr.sin_addr.s_addr = inet_addr(session->call->dip);
            session->call->addr.sin_port = htons(session->call->dport);
            session->call->rtp_session.nseqpkt = 0;
            session->call->rtp_session.firstpkt = TRUE;
            


            session->call->socket = open_udp_socket(session->localip,session->call->sport);

            l = sizeof(addr);
            //if(getsockname(session->call->socket, (struct sockaddr *) &addr, (socklen_t *) &l)) die_with_error("getsockname() //failed");



            s = mk_sip_msg(session, DEFAULT_EXPIRATION, INVITE_OK);

            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1){
                die_with_error("sendto() failed");
            }
                

            session->prev_state = REGISTERED;
            session->curr_state = INVITED;

        }
        break;
    case INVITING:
        printf("#### Case = INVITING ####\n");
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {
            printf("ACK\n");

            session->call->dport = msg->call->dport;

            session->call->tag = (char *) malloc((strlen(msg->call->tag)+1)*sizeof(char));
            memset(session->call->tag,'\0',strlen(msg->call->tag)+1);
            memcpy(session->call->tag,msg->call->tag,strlen(msg->call->tag));
            session->call->addr.sin_family = AF_INET;
            session->call->addr.sin_addr.s_addr = inet_addr(session->call->dip);
            session->call->addr.sin_port = htons(session->call->dport);



            s = mk_sip_msg(session, DEFAULT_EXPIRATION, ACK);



            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1)
                die_with_error("sendto() failed");

            free_sip_call(msg->call);

            session->prev_state = INVITING;
            session->curr_state = ONCALL;

        }
        else if(msg->type == ANSWER && msg->code == 404)
        {

            session->call->tag = (char *) malloc((strlen(msg->call->tag)+1)*sizeof(char));
            memset(session->call->tag,'\0',strlen(msg->call->tag)+1);
            memcpy(session->call->tag,msg->call->tag,strlen(msg->call->tag));
            

            s = mk_sip_msg(session, DEFAULT_EXPIRATION, ACK);

            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1){
                die_with_error("sendto() failed");
            }
                

            free_sip_call(msg->call);
            free_sip_call(session->call);
            session->call = NULL;

            fprintf(stdout,"\nNo VoIP application will be found on the destination.\n");
            session->prev_state = INVITING;
            session->curr_state = REGISTERED;
        }
        break;
    case INVITED:
        printf("#### Case = INVITED ####\n");
        
        if(msg->type == ANSWER)
        {

            session->prev_state = INVITED;
            session->curr_state = ONCALL;
        }
        free_sip_call(msg->call);
        break;

    case ONCALL:
        printf("#### Case = ONCALL ####\n");
        if(msg->type == REQUEST && !strcmp(msg->method,"BYE"))
        {
            session->seq = msg->cseq;

            s = mk_sip_msg(session, DEFAULT_EXPIRATION, BYE_OK);

            if((l = sendto(session->socket,s,strlen(s)+1,0,(struct sockaddr *) &(session->addr), sizeof(session->addr))) != strlen(s)+1){
                die_with_error("sendto() failed");
            }
                

            close(session->call->socket);
            free_sip_call(session->call);
            session->call = NULL;
            free_sip_call(msg->call);

            session->prev_state = ONCALL;
            session->curr_state = REGISTERED;
        }
        break;

    case BYE:
        printf("#### Case = BYE ####\n");
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {

            session->prev_state = BYE;
            session->curr_state = REGISTERED;
            free_sip_call(msg->call);
            free_sip_call(session->call);
            session->call = NULL;
        }
        break;
    case UNREGISTERING:
        printf("#### Case = UNREGISTERING ####\n");
        if(msg->type == ANSWER && msg->code >= 200 && msg->code < 300)
        {
            session->prev_state = REGISTERED;
            session->curr_state = UNREGISTERED;
            free_sip_call(msg->call);
        }
        break;

    }
    
    free_sip_msg(msg);
    

}

