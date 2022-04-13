/**************************FILE HEADER********************************************* 
*    FILENAME       :   main.c
*
*    DESCRIPTION    :    Call all the functions here
*                                  
*                    
*     Revision History    :
*     DATE             NAME              REFERENCE                       REASON
*     -------------------------------------------------------------------------
*     11 April  2022    Group-6       SIP Protocol Implementation        New code 
*    
*     Copyright © 2022 
*
**********************************************************************************/

#include <header.h>
#include <text.h>
#include <sip.h>
#include <rtp.h>

extern int textfd;


/************************************************************************************
*
*    FUNCTION NAME    :    usage() 
*
*    DESCRIPTION      :    If the command line arguments are not according to the 
                           required then call this function
*
*    RETURN           :     exit the whole program 
*
**************************************************************************************/
void usage()
{
    fprintf(stderr, "Usage: voz user extension@pbx_ip[:port] local_ip[:port]\n");
    exit(EXIT_FAILURE);
}

/************************************************************************************
*
*    FUNCTION NAME    :     die_with_error(const char * message)
*
*    DESCRIPTION      :    Exit the program after printing the message on the terminal.
*
*    RETURN           :     Exit the whole program
*
**************************************************************************************/
void die_with_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

/************************************************************************************
*
*    FUNCTION NAME    :     open_udp_socket(char * ip, int port)
*
*    DESCRIPTION      :    It is used to create a socket and bind that socket to a address
*
*    RETURN           :     returns the socket file descriptor(sockfd)
*
**************************************************************************************/
int open_udp_socket(char *ip, int port)
{
    int sockfd, ret;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port > 0 ? port : INADDR_ANY);

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        die_with_error("socket() failed");
    }
        

    if ((ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) < 0){
        die_with_error("bind() failed");
    }
        

    return sockfd;
}


/************************************************************************************
*
*    FUNCTION NAME    :     main(int argc, char*argv[])
*
*    DESCRIPTION      :    Recieve the arguments from the user, parse them and initialize the sip session 
                            and perform the required operations
*
*    RETURN           :     EXIT_SUCCESS on SUCCESS 
*
**************************************************************************************/
int main(int argc, char *argv[])
{
    /* verify syntax */
    if (argc != 4){
        usage();
    }
        
    char *r = NULL;
    char *t = NULL;
    char msg[MAX_SIP_LEN];
    char choice;
    char buf[BUFSIZE];

    int retVal = INIT;
    int p = INIT;
    int counter = 1;

    struct sockaddr_in from;
    socklen_t fromlen;
    fromlen = sizeof(from);

    sip_session_t *session;
    session = (sip_session_t *)malloc(sizeof(struct sip_session_t));

    fd_set ready_socket;

    session->pbxport = session->localport = -1;

    

    /* parse arguments */
    session->user = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char));
    memcpy(session->user, argv[1], strlen(argv[1]) + 1);
    r = strchr(argv[2], '@');
    if (r == NULL)
        usage();
    *r = '\0';
    session->ext = atoi(argv[2]);
    r++;
    t = strchr(r, ':');
    if (t == NULL)
        session->pbxport = DEFAULT_PORT;
    else
        *t = '\0';
    session->pbxip = (char *)malloc((strlen(r) + 1) * sizeof(char));
    memcpy(session->pbxip, r, strlen(r) + 1);
    if (session->pbxport < 0)
    {
        t++;
        session->pbxport = atoi(t);
    }
    r = strchr(argv[3], ':');
    if (r == NULL)
        session->localport = DEFAULT_PORT;
    else
        *r = '\0';
    session->localip = (char *)malloc((strlen(argv[3]) + 1) * sizeof(char));
    memcpy(session->localip, argv[3], strlen(argv[3]) + 1);
    if (session->localport < 0)
    {
        r++;
        session->localport = atoi(r);
    }
    if (argc == 5)
        session->power = atoi(argv[4]);

    

    /* open text file */
    textfd = open_file();

    session->addr.sin_family = AF_INET;
    session->addr.sin_addr.s_addr = inet_addr(session->pbxip);
    session->addr.sin_port = htons(session->pbxport);

    init_sip_session(session);

    

    while (1)

    {
        retVal = 0;

        
        printf("Session curr State is %d\t Session prev state is %d\n", session->curr_state, session->prev_state);

        if (session->prev_state != session->curr_state)
        {
            switch (session->curr_state)
            {
            case REGISTERING:
                printf("\n\t\tRegistering\n");
                break;

            case REGISTERED:
                printf("\n\t\tRegistered\n");
                fprintf(stdout, "\nL - Connect to another VoIP application\nT - End application\n\n");
                
                break;
            case ONCALL:
                printf("\n\t\tOncall\n");
                fprintf(stdout, "\nD - Turn off the voice call\n\n");
                
                break;
            case INVITING:
                printf("\n\t\tInviting\n");
                fprintf(stdout, "\nD - Turn off the voice call\n\n");
                

                break;
            case UNREGISTERED:
                fprintf(stdout, "\n Unregistered\n");
                printf("Before close\n");
                close(session->socket);
                printf("After close\n");

                if (session->call != NULL)
                {

                    free_sip_call(session->call);
                }
                free_sip_session(session);
                close(textfd);

                exit(EXIT_SUCCESS);
            }
        }

        

        FD_ZERO(&ready_socket);
        FD_SET(session->socket, &ready_socket);
        
        FD_SET(fileno(stdin), &ready_socket);
       

        if (session->curr_state == ONCALL)
        {
            printf("Trying to set the State inside ONCALL...\n");
            FD_SET(session->call->socket, &ready_socket);
            FD_SET(textfd, &ready_socket);
        }

        retVal = select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL);
        
        if(retVal < 0){
            die_with_error("select() failed\n");
        }

        if (session->curr_state == ONCALL)
        {
            
            if (FD_ISSET(session->call->socket, &ready_socket))
            {
                
                p = rtp_recvfrom(session->call->socket, buf, BUFSIZE);

                if ((retVal = write(textfd, buf, p)) < 0){
                    die_with_error("write() failed");
                }
                    
            }

            if (FD_ISSET(textfd, &ready_socket))
            {
                
                session->call->rtp_session.nseqnon++;

                if ((retVal = read(textfd, buf, BUFSIZE)) < 0){
                    die_with_error("read() failed");
                }

                if(counter == 1){

                session->call->rtp_session.nseqpkt++;
                rtp_sendto(session, buf, retVal, &(session->call->addr));
                counter--;
                }
                    
                if(counter == 0){
                    session->call->rtp_session.firstpkt = TRUE;
                    
                }
            }
        }

       

        if (FD_ISSET(session->socket, &ready_socket))
        {

            memset(msg, 0, MAX_SIP_LEN);

            if ((retVal = recvfrom(session->socket, msg, MAX_SIP_LEN, 0, (struct sockaddr *)&from, &fromlen)) < 0){
                die_with_error("recvfrom() failed");
            }
                
            
            handle_sip_msg(session, msg);
            
        }

        if (FD_ISSET(fileno(stdin), &ready_socket))
        {

            
            choice = getchar();
            

            switch (choice)
            {
            case 'L':

                if (session->curr_state == REGISTERING)
                {
                    
                    fprintf(stdout, "\nExtension: ");
                    fscanf(stdin, "%s", t);
                    fprintf(stdout, "IP: ");
                    fscanf(stdin, "%s", r);
                    fprintf(stdout, "Port: ");
                    fscanf(stdin, "%i", &p);

                    session->call = (sip_call_t *)malloc(sizeof(sip_call_t));
                    session->call->dext = (char *)malloc((strlen(t) + 1) * sizeof(char));
                    memset(session->call->dext, '\0', strlen(t) + 1);
                    memcpy(session->call->dext, t, strlen(t));
                    session->call->dip = (char *)malloc((strlen(r) + 1) * sizeof(char));
                    memcpy(session->call->dip, r, strlen(r) + 1);
                    session->call->dport = p;
                    session->call->duser = NULL;
                    session->call->id = NULL;
                    session->call->tag = NULL;
                    session->call->rtp_session.nseqpkt = 0;
                    session->call->rtp_session.nseqnon = 0;
                    session->call->rtp_session.firstpkt = TRUE;
                    printf("Sending INVITE to the server...\n");
                    
                    send_sip_invite(session);
                    
                    printf("INVITE sent successfully...\n");
                }

                break;
            case 'D':
                if (session->curr_state == ONCALL){
                    send_sip_bye(session);
                }
                    
                else if (session->curr_state == INVITING){
                    printf("I'm here and I sent a cancel\n");
                    
                }
                break;
            case 'T':
                if (session->curr_state == REGISTERED)
                    close_sip_session(session);

                break;

            default:
                printf("Invalid Choice..\n");
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}