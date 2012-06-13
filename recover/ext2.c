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
    int offset = inode_id % inodes_per_block;

    if (first_group != group) {
        first_id = -100;
        first_group = group;
    }
    //printf("gruop:%d id:%d\n",group,inode_id);
    if (inode_id < first_id || inode_id > first_id + 15) {

        //    printf("%d %d\n",block,inodes_buf);
        if (mdev_readblock(block, inodes_buf)) {
            //  printf("%d %d\n",group,inode_id);
            printf("error read\n");
            return NULL;
        }
        first_id = inode_id;

    }


    if (inodes_buf[offset].i_dtime == 0) {
	//  printf("UNdelete Inode %d there are %d databolck\n", group * superblock.s_inodes_per_group + inode_id + 1,  inodes_buf[offset].i_size / blocksize + 1);
      //  int i;
     //   for (i = 0; i < EXT2_N_BLOCKS; i++) {
      //      printf("%d, ", inodes_buf[offset].i_block[i]);
      //  }

      //  printf("\n");
        return NULL;
    } else {
	time_t dt=inodes_buf[offset].i_dtime;
        //printf("size:%d\n",inodes_buf[offset].i_size);
        if (inodes_buf[offset].i_size == 0)//why a inode has no data with a deletiem????????????????
            return NULL;
        printf("Inode %d Deleted at %s,there are %d databolck %d bytes\n", group * superblock.s_inodes_per_group + inode_id + 1, ctime(&dt), inodes_buf[offset].i_size / blocksize + 1,inodes_buf[offset].i_size);
        int i;
        for (i = 0; i < EXT2_N_BLOCKS; i++) {
            printf("%d, ", inodes_buf[offset].i_block[i]);
        }

        printf("\n");
        return &inodes_buf[offset];

    }
}

int undelete(struct ext2_inode *d_inode, int id) {
   

    char filename[50] = "./inode";
    char temp[10];
    sprintf(temp, "%d", id+1);
    strcat(filename, temp);

    int file = creat(filename, 777);
    char *buf = (char *) malloc(blocksize);

    int block_num = d_inode->i_size / blocksize + 1;
    int size = 0;
    int i;
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
    char cmd[100] = "chmod 777 ";
    strcat(cmd, filename);
    system(cmd);
	if(block_num>12)
   		 printf("The data in first 12 direct block is undeleted!\n\n");
	else 
		 printf("The file  is undeleted!\n\n");

}

void scan_node() {

    int i;
    printf("scaning...\n");
    for (i = 0; i < block_groups_count; i++) {
        int j, cur, tail;

        mdev_readblock(group_descriptors[i].bg_inode_bitmap, buffer);

        for (j = 0, cur = 0, tail = superblock.s_inodes_per_group; cur < tail; j++) {
            int k;
            for (k = 0; k < 8 && cur < tail; k++, cur++) {

                if ((buffer[j]&(1 << k)) == 0) {
                    struct ext2_inode *d_node = NULL;
                    d_node = is_dnode(i, cur);

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

