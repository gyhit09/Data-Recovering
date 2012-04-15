#ifndef READEV_H
#define READEV_H


int fs_open      (char * path);
int fs_close     ();
int fs_readblock (int block, void * buf);

#endif
