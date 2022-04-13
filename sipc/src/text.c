/********************************************************************************* 
*    FILENAME       :    text.c
*
*    DESCRIPTION    :    Contains functions to open the file and close the file
*                        	       
*                    
*     Revision History    :
*     DATE             NAME              REFERENCE                   REASON
*     --------------------------------------------------------------------------
*     06-April-2022    Group-6          SIP Protocol Implementation         Initial Creation
*      
*     Copyright © 2022 
*
**********************************************************************************/

#include <header.h>
#include <text.h>

int textfd;

/************************************************************************************
*
*    FUNCTION NAME    :    open_file() 
*
*    DESCRIPTION      :   Open the file and return the file descriptor to the Callee function 
*
*    RETURN           :     file descriptor(fd)
*
**************************************************************************************/
int open_file()
{
    struct  stat st;
    
    int err = stat(FILENAME, &st);
    if (err < 0)
    {
        fprintf(stderr, "cannot stat text file\n");
        exit(EXIT_FAILURE);
    }
    int fd, sz;
    char *c = (char *)calloc(100, sizeof(char));

    fd = open(FILENAME, O_RDWR);
    if (fd < 0)
    {
        perror("Unable to open the file\n");
        exit(EXIT_FAILURE);
    }

    sz = read(fd, c, 100);
    
    c[sz] = '\0';
    
    return fd;
}

/************************************************************************************
*
*    FUNCTION NAME    :     close
*
*    DESCRIPTION      :    close the opened file
*
*    RETURN           :     Nothing
*
**************************************************************************************/
void close_file(int fd)
{
    close(fd);
}
