#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"
#include <string.h>


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1 || inumber > fs->superblock.s_isize * 16) {
        return -1;
    }

    int inodes_per_sector = DISKIMG_SECTOR_SIZE / 32; // 16
    int sector = INODE_START_SECTOR + (inumber - 1) / inodes_per_sector;
    int offset = ((inumber - 1) % inodes_per_sector) * 32;

    char buffer[DISKIMG_SECTOR_SIZE];
    if (diskimg_readsector(fs->dfd, sector, buffer) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    memcpy(inp, buffer + offset, sizeof(struct inode));

    if (!(inp->i_mode & IALLOC)) {
        return -1;
    }

    return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (blockNum < 0) {
        return -1;
    }

    if (!(inp->i_mode & ILARG)) {
        if (blockNum < 8) {
            return inp->i_addr[blockNum];
        }
        return -1;
    }

    int entries_per_block = DISKIMG_SECTOR_SIZE / 2; // 256
    if (blockNum < 7 * entries_per_block) {
        int indirect_idx = blockNum / entries_per_block;
        int offset = blockNum % entries_per_block;
        uint16_t buffer[entries_per_block];
        if (diskimg_readsector(fs->dfd, inp->i_addr[indirect_idx], buffer) != DISKIMG_SECTOR_SIZE) {
            return -1;
        }
        return buffer[offset];
    } else {
        int adjusted = blockNum - 7 * entries_per_block;
        int first_idx = adjusted / entries_per_block;
        int second_idx = adjusted % entries_per_block;
        uint16_t double_indirect[entries_per_block];
        if (diskimg_readsector(fs->dfd, inp->i_addr[7], double_indirect) != DISKIMG_SECTOR_SIZE) {
            return -1;
        }
        if (double_indirect[first_idx] == 0) {
            return -1;
        }
        uint16_t indirect[entries_per_block];
        if (diskimg_readsector(fs->dfd, double_indirect[first_idx], indirect) != DISKIMG_SECTOR_SIZE) {
            return -1;
        }
        return indirect[second_idx];
    }
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
