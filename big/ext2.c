#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<time.h>
#include<fcntl.h>
#include <linux/types.h>
#include "ext2.h"

int mdev_close() {
    close(fd);
}

int mdev_readblock(int block, void *buf) {
    int done;
    long pos = (long) block*blocksize;
    pos = lseek(fd, pos, SEEK_SET);
    //printf("pos:%d\n",pos/blocksize);
    done = read(fd, buf, blocksize);
    // done = pread(fd, buf, blocksize, block * blocksize);
    if (done != blocksize) {
        fprintf(stderr, "Error reading\n");

        return 1;
    }
    return 0;
}

int init(char* path) {

    int i;

    blocksize = 1024;
    fd = open(path, O_RDONLY);
    if (fd != -1) {
        mdev_readblock(1, &superblock);

    }

    blocksize = 1 << (10 + superblock.s_log_block_size);
    inodes_per_block = blocksize / superblock.s_inode_size;
    block_groups_count = (superblock.s_blocks_count - 1) / superblock.s_blocks_per_group + 1;
    group_descriptor_table_size = (block_groups_count * 32 - 1) / blocksize + 1;
    block_bitmap_size = (superblock.s_blocks_per_group - 1) / (8 * blocksize) + 1;
    inode_bitmap_size = (superblock.s_inodes_per_group - 1) / (8 * blocksize) + 1;


    group_descriptors = (struct ext2_group_desc *) malloc(group_descriptor_table_size * blocksize);
    buffer = (char *) malloc(blocksize);
    for (i = 0; i < group_descriptor_table_size; i++) {
        mdev_readblock(BASE_BLOCK(0) + 1 + i, &(group_descriptors[i * blocksize / 32]));
    }
    return 0;

}

int handleInput() {
    char in[100];
    while (1) {
        scanf("%s", in);
        if (strcmp(in, "Y") == 0 || strcmp(in, "y") == 0) {
            return 1;
        } else if (strcmp(in, "N") == 0 || strcmp(in, "n") == 0) {
            return 0;
        } else {
            printf("wrong parama!\ninput again:");
        }
    }
}



struct ext2_inode inodes_buf[INODE_PER_BLOCK ]; //inode缓存区
int first_id = -100; //缓存区起始inode号
int first_group = -1;

void print_buf() {

}

struct ext2_inode* is_dnode(int group, int inode_id) {
    int block = inode_id / inodes_per_block + group_descriptors[group].bg_inode_table;
    
    //inode bitmap start address
    int offset = inode_id % inodes_per_block;
//inode bitmap number

    //printf("gruop:%d id:%d\n",group,inode_id);
 

        //    printf("%d %d\n",block,inodes_buf);
         mdev_readblock(block, inodes_buf);

 
    if (inodes_buf[offset].i_dtime != 0) {
	//  printf("UNdelete Inode %d there are %d databolck\n", group * superblock.s_inodes_per_group + inode_id + 1,  inodes_buf[offset].i_size / blocksize + 1);
      //  int i;
     //   for (i = 0; i < EXT2_N_BLOCKS; i++) {
      //      printf("%d, ", inodes_buf[offset].i_block[i]);
      //  }

      //  printf("\n");
        if (inodes_buf[offset].i_size == 0)//why a inode has no data with a deletiem????????????????
            return NULL;
        time_t dt=inodes_buf[offset].i_dtime;
        printf("Inode %d Deleted at %s,there are %d databolck %d bytes\n", group * superblock.s_inodes_per_group + inode_id + 1, ctime(&dt), inodes_buf[offset].i_size / blocksize + 1,inodes_buf[offset].i_size);
      return &inodes_buf[offset];
       
    }

    return NULL;
        

    }


int undelete(struct ext2_inode *d_inode, int id) {
   

    char filename[50] = "./inode";
    char temp[10];
    sprintf(temp, "%d", id+1);
    strcat(filename, temp);

    int file = creat(filename, 777);
    char *buf = (char *) malloc(blocksize);
    unsigned int *buf1 = (unsigned int *) malloc(blocksize);
    unsigned int *buf2 = (unsigned int *) malloc(blocksize);
    unsigned int *buf3 = (unsigned int *) malloc(blocksize);
    int block_num = d_inode->i_size / blocksize + 1;
    int size = 0;
    int i,j,k;
    
	//printf("the block_num is %d\n",block_num);
    for (i = 0; i< block_num&& i<12; i++) {
	if(d_inode->i_block[i]==0)
		{ 
			//printf("block %d is hole\n",i);
			size = lseek(file,  blocksize,SEEK_CUR);
			continue;
		}
        mdev_readblock(d_inode->i_block[i], buf);
        if ((d_inode->i_size - size) < blocksize) {
            write(file, buf, d_inode->i_size - size);
        } else {
            size += write(file, buf, blocksize);
        }
	//printf("block %d is undeleted\n",i);
    }
	if(block_num>11){
	if(d_inode->i_block[12]!=0){
	mdev_readblock(d_inode->i_block[12],buf1);
	for(i=0;i<blocksize/4&&buf1[i];i++){
	mdev_readblock(buf1[i],buf);
	write(file,buf,blocksize);
	}}/*               1  indirect inode  recover*/
	if(d_inode->i_block[13]!=0){
	mdev_readblock(d_inode->i_block[13],buf1);
	for(i=0;i<blocksize/4&&buf1[i];i++){
	mdev_readblock(buf1[i],buf2);
	for(j=0;j<blocksize/4&&buf2[j];j++)
	{
	mdev_readblock(buf2[j],buf);
	write(file,buf,blocksize);	
	}
	}}/*              2 indirect inode  recover*/
	if(d_inode->i_block[14]!=0){	
	mdev_readblock(d_inode->i_block[14],buf1);
	for(i=0;i<blocksize/4&&buf1[i];i++){
	mdev_readblock(buf1[i],buf2);
	for(j=0;j<blocksize/4&&buf2[j];j++)
	{
	mdev_readblock(buf2[j],buf3);
	for(k=0;k<blocksize/4&&buf3[k];k++)
	mdev_readblock(buf3[k],buf);
	write(file,buf,blocksize);		
	}/*               3 indirect inode recover */
	}	
	}}
    char cmd[100] = "chmod 777 ";
    strcat(cmd, filename);
    system(cmd);
	/*if(block_num>12)
   		 printf("The data in first 12 direct block is undeleted!\n\n");
	else 
		 printf("The file  is undeleted!\n\n");
*/
}

void scan_node() {

    int i;
    printf("scaning...\n");
    for (i = 0; i < block_groups_count; i++) {
        int j, cur, tail;
//printf("%d",block_groups_count);
        mdev_readblock(group_descriptors[i].bg_inode_bitmap, buffer);

        for (j = 0, cur = 0, tail = superblock.s_inodes_per_group; cur < tail; j++) {
            int k;
            
            for (k = 0; k < 8 && cur < tail; k++, cur++) {

                if ((buffer[j]&(1 << k)) == 0) {
                    struct ext2_inode *d_node = NULL;
                    d_node = is_dnode(i, cur);
//printf("%d",cur);
                    if (d_node != NULL) {
                        printf("Do you want to try to undelete it?(Y/N)");
                        if (handleInput()) {
                            undelete(d_node, cur + i * superblock.s_inodes_per_group);
                        }
                        printf("scaning...\n");
                    }
                }
            }
        }
    }
    printf("Done\n");
}

