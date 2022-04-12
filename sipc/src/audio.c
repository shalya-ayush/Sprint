
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include "../include/audio.h"

int audiofd;


int open_audio()
{
    struct  stat st;
    
    int err = stat(FILENAME, &st);
    if (err < 0)
    {
        fprintf(stderr, "cannot stat audio device");
        exit(1);
    }
    int fd, sz;
    char *c = (char *)calloc(100, sizeof(char));

    fd = open(FILENAME, O_RDWR);
    if (fd < 0)
    {
        perror("r1");
        exit(1);
    }

    sz = read(fd, c, 100);
    // printf("called read(%d, c, 100). returned that"
    //        " %d bytes were read.\n",
    //        fd, sz);
    c[sz] = '\0';
    // printf("Those bytes are as follows: %s\n", c);
    // close(fd);
    return fd;
}
// }int arg, err, fd=-1;
//     struct stat st;

//     /* stat the device */
//     err = stat(AUDIODEVICE, &st);
//     if (err < 0)
//     {
//         fprintf(stderr,"cannot stat audio device");
//         exit(1);
//     }
//     /*  verify if is a character device */
//     if (!S_ISCHR(st.st_mode))
//     {
//         fprintf(stderr,"%s is not an audio device\n", AUDIODEVICE);
//         exit(1);
//     }
//     /* try to open the device, otherwise*/
//     fd= open(AUDIODEVICE, AUDIOMODE /* | O_NONBLOCK */);
//     if ((fd < 0) && (errno == EBUSY))
//     {
//         printf ("%s is busy\nwaiting...\n", AUDIODEVICE);

//         /* Now hang until it's open */
//         /* blocking open will wait until other applications stop using it */
//         fd= open(AUDIODEVICE, AUDIOMODE);
//     }

//     if (fd < 0)
//     {
//         printf("error opening audio device");
//         exit(1);
//     }

//     arg=0x00020008;

//     if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &arg))
//         printf("audio_open: ioctl\n");

//     return fd;
// }


void close_audio(int fd)
{
    close(fd);
}




