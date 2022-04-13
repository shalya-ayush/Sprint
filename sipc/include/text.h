#ifndef __TEXT_H__
#define __TEXT_H__

/*Change the Path according to the System */
#define FILENAME "/home/ayush/abc.txt"
#define BUFSIZE 160
#define FILEMODE O_RDWR

extern int open_file();
extern void close_file(int fd);

#endif
