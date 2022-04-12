

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
#include "../include/audio.h"
#include "../include/sip.h"
#include "../include/rtp.h"
#include "../include/g711.h"

extern int audiofd;

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
void usage()
{
    fprintf(stderr, "Usage: voz user extension@pbx_ip[:port] local_ip[:port]\n");
    exit(1);
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
void die_with_error(const char *message)
{
    perror(message);
    exit(1);
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int open_udp_socket(char *ip, int port)
{
    int s, i;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port > 0 ? port : INADDR_ANY);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        die_with_error("socket() failed");

    if ((i = bind(s, (struct sockaddr *)&addr, sizeof(addr))) < 0)
        die_with_error("bind() failed");

    return s;
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int get_power(char *buf)
{
    int a, i, p;

    for (a = 0, i = 0, p = 0; i < BUFSIZE; i++)

        a = a + (st_ulaw2linear16(buf[i]) * st_ulaw2linear16(buf[i]));

    p = a / BUFSIZE;

    return p;
}

/*****************************************************************************
 * @name
 * @description
 * @param
 * @return
 */
int main(int argc, char **argv)
{
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    char *r, *t, msg[MAX_SIP_LEN], c, buf[BUFSIZE];
    int n = 0, p;

    struct sockaddr_in from;
    socklen_t fromlen;
    fromlen = sizeof(from);

    sip_session_t *session;
    session = (sip_session_t *)malloc(sizeof(struct sip_session_t));

    fd_set mask;

    session->pbxport = session->localport = -1;

    /* verify syntax */
    if (argc != 4 && argc != 5)
        usage();

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

    printf("Step-1: Arguments has beeen parsed Successfully..\n\n");

    printf("Step:2 Trying to read the text..\n\n");

    /* open audio device */
    audiofd = open_audio();

    session->addr.sin_family = AF_INET;
    session->addr.sin_addr.s_addr = inet_addr(session->pbxip);
    session->addr.sin_port = htons(session->pbxport);

    init_sip_session(session);

    while (1)

    {
        n = 0;

        printf("Inside While Loop...\n");
        printf("Session curr State is %d\t Session prev state is %d\n", session->curr_state, session->prev_state);

        if (session->prev_state != session->curr_state)
        {
            switch (session->curr_state)
            {
            case REGISTERING:
                printf("Registering\n");
                break;

            case REGISTERED:
                fprintf(stdout, "\nL - Connect to another VoIP application\nT - End application\n\n");
                printf("Registered\n");
                break;
            case ONCALL:
                fprintf(stdout, "\nD - Turn off the voice call\n\n");
                printf("Oncall\n");
                break;
            case INVITING:
                fprintf(stdout, "\nD - Turn off the voice call\n\n");
                printf("Inviting\n");

                break;
            case UNREGISTERED:
                printf("UnRegistered\n");
                fprintf(stdout, "\n Unregistered\n");
                printf("Before close\n");
                close(session->socket);
                printf("After close\n");

                if (session->call != NULL)
                {

                    free_sip_call(session->call);
                }
                free_sip_session(session);
                close(audiofd);

                exit(0);
            }
        }

        /*TODO */

        FD_ZERO(&mask);
        FD_SET(session->socket, &mask);
        //////
        FD_SET(fileno(stdin), &mask);
        //////

        if (session->curr_state == ONCALL)
        {
            printf("Trying to set the State inside ONCALL...\n");
            FD_SET(session->call->socket, &mask);
            FD_SET(audiofd, &mask);
        }

        int retval = select(FD_SETSIZE, &mask, NULL, NULL, NULL);
        if (retval == -1)
            perror("select()");
        else
            printf("Data is available now.\n");

        printf("Select() is called...\n");

        if (session->curr_state == ONCALL)
        {
            printf("Inside ONCALL State 1..\n");

            if (FD_ISSET(session->call->socket, &mask))
            {

                p = rtp_recvfrom(session->call->socket, buf, BUFSIZE);

                if ((n = write(audiofd, buf, p)) < 0)
                    die_with_error("write() failed");
            }

            if (FD_ISSET(audiofd, &mask))
            {
                printf("Inside ONCALL state 2...\n");
                session->call->rtp_session.nseqnon++;

                if ((n = read(audiofd, buf, BUFSIZE)) < 0)
                    die_with_error("read() failed");

                //     if ((n = write(audiofd, buf, BUFSIZE)) < 0)
                //         die_with_error("write() failed");

                if (session->power < get_power(buf))
                {
                    printf("%i\n", get_power(buf));
                    session->call->rtp_session.nseqpkt++;
                    rtp_sendto(session, buf, n, &(session->call->addr));
                }
                else
                    session->call->rtp_session.firstpkt = TRUE;
            }
        }

        // FD_SET(session->socket, &mask);

        if (FD_ISSET(session->socket, &mask))
        {

            memset(msg, 0, MAX_SIP_LEN);
            printf("Waiting for the response from the server:\n ");

            if ((n = recvfrom(session->socket, msg, MAX_SIP_LEN, 0, (struct sockaddr *)&from, &fromlen)) < 0)
                die_with_error("recvfrom() failed");
            printf("Handling SIP message function..\n");
            handle_sip_msg(session, msg);
        }

        if (FD_ISSET(fileno(stdin), &mask))
        {

            printf("Mask = %d inside fd_isset \n", mask);
            printf("Enter your Choice: ");
            __fpurge(stdin);
            c = getchar();
            printf("Choice = %c\n", c);

            switch (c)
            {
            case 'L':

                if (session->curr_state == REGISTERING)
                {
                    printf("Inside REgistering Case...\n");
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
                    printf("Send sip_invite called...\n");
                    send_sip_invite(session);
                    printf("After sip_invite function..\n");
                }

                break;
            case 'D':
                if (session->curr_state == ONCALL)
                    send_sip_bye(session);
                else if (session->curr_state == INVITING)
                    printf("I'm here and I sent a cancel\n");
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

    return 0;
}