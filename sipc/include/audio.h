#ifndef __AUDIO_H__
#define __AUDIO_H__

#define FILENAME "/mnt/c/Users/shkarn/sprintCG/Build Project/abc.txt"
#define BUFSIZE 160
#define FILEMODE O_RDWR

extern int open_audio();
extern void close_audio(int fd);

#endif
