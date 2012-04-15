#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/types.h>
#define EXT2_N_BLOCKS 15
#define BASE_BLOCK(group) (superblock.s_first_data_block \
	+ superblock.s_blocks_per_group * (group) )
#include "readev.h"
struct ext2_super_block
{
    __u32	s_inodes_count;		/* Inodes count */
    __u32	s_blocks_count;		/* Blocks count */
    __u32	s_r_blocks_count;	/* Reserved blocks count */
    __u32	s_free_blocks_count;	/* Free blocks count */
    __u32	s_free_inodes_count;	/* Free inodes count */
    __u32	s_first_data_block;	/* First Data Block */
    __u32	s_log_block_size;	/* Block size */
    __s32	s_log_frag_size;	/* Fragment size */
    __u32	s_blocks_per_group;	/* # Blocks per group */
    __u32	s_frags_per_group;	/* # Fragments per group */
    __u32	s_inodes_per_group;	/* # Inodes per group */
    __u32	s_mtime;		/* Mount time */
    __u32	s_wtime;		/* Write time */
    __u16	s_mnt_count;		/* Mount count */
    __s16	s_max_mnt_count;	/* Maximal mount count */
    __u16	s_magic;		/* Magic signature */
    __u16	s_state;		/* File system state */
    __u16	s_errors;		/* Behaviour when detecting errors */
    __u16	s_minor_rev_level; 	/* minor revision level */
    __u32	s_lastcheck;		/* time of last check */
    __u32	s_checkinterval;	/* max. time between checks */
    __u32	s_creator_os;		/* OS */
    __u32	s_rev_level;		/* Revision level */
    __u16	s_def_resuid;		/* Default uid for reserved blocks */
    __u16	s_def_resgid;		/* Default gid for reserved blocks */
    /*
     * These fields are for EXT2_DYNAMIC_REV superblocks only.
     *
     * Note: the difference between the compatible feature set and
     * the incompatible feature set is that if there is a bit set
     * in the incompatible feature set that the kernel doesn't
     * know about, it should refuse to mount the filesystem.
     *
     * e2fsck's requirements are more strict; if it doesn't know
     * about a feature in either the compatible or incompatible
     * feature set, it must abort and not try to meddle with
     * things it doesn't understand...
     */
    __u32	s_first_ino; 		/* First non-reserved inode */
    __u16   s_inode_size; 		/* size of inode structure */
    __u16	s_block_group_nr; 	/* block group # of this superblock */
    __u32	s_feature_compat; 	/* compatible feature set */
    __u32	s_feature_incompat; 	/* incompatible feature set */
    __u32	s_feature_ro_compat; 	/* readonly-compatible feature set */
    __u8	s_uuid[16];		/* 128-bit uuid for volume */
    char	s_volume_name[16]; 	/* volume name */
    char	s_last_mounted[64]; 	/* directory where last mounted */
    __u32	s_algorithm_usage_bitmap; /* For compression */
    /*
     * Performance hints.  Directory preallocation should only
     * happen if the EXT2_COMPAT_PREALLOC flag is on.
     */
    __u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
    __u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
    __u16	s_padding1;
    /*
     * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
     */
    __u8	s_journal_uuid[16];	/* uuid of journal superblock */
    __u32	s_journal_inum;		/* inode number of journal file */
    __u32	s_journal_dev;		/* device number of journal file */
    __u32	s_last_orphan;		/* start of list of inodes to delete */
    __u32	s_hash_seed[4];		/* HTREE hash seed */
    __u8	s_def_hash_version;	/* Default hash version to use */
    __u8	s_reserved_char_pad;
    __u16	s_reserved_word_pad;
    __u32	s_default_mount_opts;
    __u32	s_first_meta_bg; 	/* First metablock block group */
    __u32	s_reserved[190];	/* Padding to the end of the block */
};


struct ext2_group_desc
{
    __u32	bg_block_bitmap;	/* Blocks bitmap block */
    __u32	bg_inode_bitmap;	/* Inodes bitmap block */
    __u32	bg_inode_table;		/* Inodes table block */
    __u16	bg_free_blocks_count;	/* Free blocks count */
    __u16	bg_free_inodes_count;	/* Free inodes count */
    __u16	bg_used_dirs_count;	/* Directories count */
    __u16	bg_pad;
    __u32	bg_reserved[3];
};


struct ext2_inode
{
    __le16	i_mode;		/* File mode */
    __le16	i_uid;		/* Low 16 bits of Owner Uid */
    __le32	i_size;		/* Size in bytes */
    __le32	i_atime;	/* Access time */
    __le32	i_ctime;	/* Creation time */
    __le32	i_mtime;	/* Modification time */
    __le32	i_dtime;	/* Deletion Time */
    __le16	i_gid;		/* Low 16 bits of Group Id */
    __le16	i_links_count;	/* Links count */
    __le32	i_blocks;	/* Blocks count */
    __le32	i_flags;	/* File flags */
    union
    {
        struct
        {
            __le32  l_i_reserved1;
        } linux1;
        struct
        {
            __le32  h_i_translator;
        } hurd1;
        struct
        {
            __le32  m_i_reserved1;
        } masix1;
    } osd1;				/* OS dependent 1 */
    __le32	i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
    __le32	i_generation;	/* File version (for NFS) */
    __le32	i_file_acl;	/* File ACL */
    __le32	i_dir_acl;	/* Directory ACL */
    __le32	i_faddr;	/* Fragment address */
    union
    {
        struct
        {
            __u8	l_i_frag;	/* Fragment number */
            __u8	l_i_fsize;	/* Fragment size */
            __u16	i_pad1;
            __le16	l_i_uid_high;	/* these 2 fields    */
            __le16	l_i_gid_high;	/* were reserved2[0] */
            __u32	l_i_reserved2;
        } linux2;
        struct
        {
            __u8	h_i_frag;	/* Fragment number */
            __u8	h_i_fsize;	/* Fragment size */
            __le16	h_i_mode_high;
            __le16	h_i_uid_high;
            __le16	h_i_gid_high;
            __le32	h_i_author;
        } hurd2;
        struct
        {
            __u8	m_i_frag;	/* Fragment number */
            __u8	m_i_fsize;	/* Fragment size */
            __u16	m_pad1;
            __u32	m_i_reserved2[2];
        } masix2;
    } osd2;				/* OS dependent 2 */
};

struct ext2_super_block superblock;
struct ext2_group_desc * group_descriptors;
int blocksize = 1024;
int groups_count;
int group_descriptor_table_size;
int block_bitmap_size;
int inode_bitmap_size;
char  * buffer;
int open_dev (char * path)
{
    int i;
    if (fs_open (path) ) return 1;

    fs_read_block (1, &superblock);

    /* 为0则block大小为1024，为1则block大小为2048，为2则block大小为4096*/
    blocksize = 1 << (10 + superblock.s_log_block_size);

    groups_count = ( (superblock.s_blocks_count - 1)
                     /superblock.s_blocks_per_group) + 1;


    group_descriptor_table_size =
        (groups_count * 32 - 1) / blocksize + 1;
    /*32 bit-mapping*/

    /*分区中的块组数=分区大小/（块大小*8）*/
    block_bitmap_size = (superblock.s_blocks_per_group - 1)
                        / (8 * blocksize) + 1;
    inode_bitmap_size = (superblock.s_inodes_per_group - 1)
                        / (8 * blocksize) + 1;

    buffer = (char  *) malloc (blocksize);
    group_descriptors = (struct ext2_group_desc *)
                        malloc (group_descriptor_table_size * blocksize);

    for (i = 0; i < group_descriptor_table_size; i++)
    {
        fs_read_block (BASE_BLOCK (0) + 1 + i,
                       &(group_descriptors[i * blocksize / 32]));
    }

    return 0;
}
int main (int argc, char * argv[])
{
    int ret;

    if (argc < 2)
    {
        printf ("Parameter Missing!\n");
        return 1;
    }

    ret = open_dev (argv[1]);

    if (ret)
        return ret;
}
