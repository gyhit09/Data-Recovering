/* 
 * File:   ext2.h
 * Author: root
 *花了我好几天改的一个错误是，在mdev_readblock和mdev_read函数中用lssek定位数据的时候没考虑到字节数会超过32位int的范围，改正方法就是用long来表示数据位置
 * Created on 2012年4月23日, 下午3:07
 */

#ifndef EXT2_H
#define	EXT2_H

#ifdef	__cplusplus
extern "C" {
#endif


#define EXT2_N_BLOCKS 15
#define INODE_PER_BLOCK 64

/*
     * Structure of the super block
     */

    struct ext2_super_block {
        __le32 s_inodes_count; /* Inodes count */
        __le32 s_blocks_count; /* Blocks count */
        __le32 s_r_blocks_count; /* Reserved blocks count */
        __le32 s_free_blocks_count; /* Free blocks count */
        __le32 s_free_inodes_count; /* Free inodes count */
        __le32 s_first_data_block; /* First Data Block */
        __le32 s_log_block_size; /* Block size */
        __le32 s_log_frag_size; /* Fragment size */
        __le32 s_blocks_per_group; /* # Blocks per group */
        __le32 s_frags_per_group; /* # Fragments per group */
        __le32 s_inodes_per_group; /* # Inodes per group */
        __le32 s_mtime; /* Mount time */
        __le32 s_wtime; /* Write time */
        __le16 s_mnt_count; /* Mount count */
        __le16 s_max_mnt_count; /* Maximal mount count */
        __le16 s_magic; /* Magic signature */
        __le16 s_state; /* File system state */
        __le16 s_errors; /* Behaviour when detecting errors */
        __le16 s_minor_rev_level; /* minor revision level */
        __le32 s_lastcheck; /* time of last check */
        __le32 s_checkinterval; /* max. time between checks */
        __le32 s_creator_os; /* OS */
        __le32 s_rev_level; /* Revision level */
        __le16 s_def_resuid; /* Default uid for reserved blocks */
        __le16 s_def_resgid; /* Default gid for reserved blocks */
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
        __le32 s_first_ino; /* First non-reserved inode */
        __le16 s_inode_size; /* size of inode structure */
        __le16 s_block_group_nr; /* block group # of this superblock */
        __le32 s_feature_compat; /* compatible feature set */
        __le32 s_feature_incompat; /* incompatible feature set */
        __le32 s_feature_ro_compat; /* readonly-compatible feature set */
        __u8 s_uuid[16]; /* 128-bit uuid for volume */
        char s_volume_name[16]; /* volume name */
        char s_last_mounted[64]; /* directory where last mounted */
        __le32 s_algorithm_usage_bitmap; /* For compression */
        /*
         * Performance hints.  Directory preallocation should only
         * happen if the EXT2_COMPAT_PREALLOC flag is on.
         */
        __u8 s_prealloc_blocks; /* Nr of blocks to try to preallocate*/
        __u8 s_prealloc_dir_blocks; /* Nr to preallocate for dirs */
        __u16 s_padding1;
        /*
         * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
         */
        __u8 s_journal_uuid[16]; /* uuid of journal superblock */
        __u32 s_journal_inum; /* inode number of journal file */
        __u32 s_journal_dev; /* device number of journal file */
        __u32 s_last_orphan; /* start of list of inodes to delete */
        __u32 s_hash_seed[4]; /* HTREE hash seed */
        __u8 s_def_hash_version; /* Default hash version to use */
        __u8 s_reserved_char_pad;
        __u16 s_reserved_word_pad;
        __le32 s_default_mount_opts;
        __le32 s_first_meta_bg; /* First metablock block group */
        __u32 s_reserved[190]; /* Padding to the end of the block */
    };

    /*
     * Structure of a blocks group descriptor
     */

    struct ext2_group_desc {
        __le32 bg_block_bitmap; /* Blocks bitmap block */
        __le32 bg_inode_bitmap; /* Inodes bitmap block */
        __le32 bg_inode_table; /* Inodes table block */
        __le16 bg_free_blocks_count; /* Free blocks count */
        __le16 bg_free_inodes_count; /* Free inodes count */
        __le16 bg_used_dirs_count; /* Directories count */
        __le16 bg_pad;
        __le32 bg_reserved[3];
    };

    /*
     * Structure of an inode on the disk
     */

    struct ext2_inode {
        __le16 i_mode; /* File mode */
        __le16 i_uid; /* Low 16 bits of Owner Uid */
        __le32 i_size; /* Size in bytes */
        __le32 i_atime; /* Access time */
        __le32 i_ctime; /* Creation time */
        __le32 i_mtime; /* Modification time */
        __le32 i_dtime; /* Deletion Time */
        __le16 i_gid; /* Low 16 bits of Group Id */
        __le16 i_links_count; /* Links count */
        __le32 i_blocks; /* Blocks count */
        __le32 i_flags; /* File flags */

        union {

            struct {
                __le32 l_i_reserved1;
            } linux1;

            struct {
                __le32 h_i_translator;
            } hurd1;

            struct {
                __le32 m_i_reserved1;
            } masix1;
        } osd1; /* OS dependent 1 */
        __le32 i_block[EXT2_N_BLOCKS]; /* Pointers to blocks */
        __le32 i_generation; /* File version (for NFS) */
        __le32 i_file_acl; /* File ACL */
        __le32 i_dir_acl; /* Directory ACL */
        __le32 i_faddr; /* Fragment address */

        union {

            struct {
                __u8 l_i_frag; /* Fragment number */
                __u8 l_i_fsize; /* Fragment size */
                __u16 i_pad1;
                __le16 l_i_uid_high; /* these 2 fields    */
                __le16 l_i_gid_high; /* were reserved2[0] */
                __u32 l_i_reserved2;
            } linux2;

            struct {
                __u8 h_i_frag; /* Fragment number */
                __u8 h_i_fsize; /* Fragment size */
                __le16 h_i_mode_high;
                __le16 h_i_uid_high;
                __le16 h_i_gid_high;
                __le32 h_i_author;
            } hurd2;

            struct {
                __u8 m_i_frag; /* Fragment number */
                __u8 m_i_fsize; /* Fragment size */
                __u16 m_pad1;
                __u32 m_i_reserved2[2];
            } masix2;
        } osd2; /* OS dependent 2 */
        __u8 reserve[128]; //为了补足256字节，加上128字节凑数
    };
    int blocksize;
    int block_groups_count;
    int group_descriptor_table_size;
    int block_bitmap_size;
    int inode_bitmap_size;
    int inodes_per_block;
    struct ext2_super_block superblock;
    struct ext2_group_desc * group_descriptors;

#define BASE_BLOCK(group) (superblock.s_first_data_block \
		+ superblock.s_blocks_per_group * (group) )


    char * buffer;
    short * status;

    int fd;
    int mdev_close();
    int mread(int block, int offset, void *buf, int size);
    int mdev_readblock(int block, void *buf);
    int init(char* path);
    int handleInput();
    void scan_node();
    struct ext2_inode* is_dnode(int group, int inode_id);
    int undelete(struct ext2_inode *d_inode, int id);




#ifdef	__cplusplus
}
#endif

#endif	/* EXT2_H */
