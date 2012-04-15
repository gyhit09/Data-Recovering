#include <stdio.h>

FILE * fs;
/* Using for opening File System */
extern const int blocksize;

/* opens filesystem */
int fs_open (char *path)
{
    if ( (fs = fopen(path,"r") ) == NULL)
    {
        printf("Can't open filesystem! ");
        return -1;
    }

    return 0;
}


/* closes filesystem */
int fs_close ()
{
    fclose (fs);
    return 0;
}

int fs_read_block (int block, void *buf)
{
    int bytes;

    bytes = fread (buf, 1, blocksize, fs);

    if (bytes != blocksize)
    {
        printf ("Read error!\n");
        return 1;
    }

    return 0;
}
