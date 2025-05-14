#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    if (blockNum < 0) {
        return -1;
    }

    struct inode inp;
    if (inode_iget(fs, inumber, &inp) != 0 || !(inp.i_mode & IALLOC)) {
        return -1;
    }

    int sector = inode_indexlookup(fs, &inp, blockNum);
    if (sector <= 0) {
        return -1;
    }

    if (diskimg_readsector(fs->dfd, sector, buf) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    int size = inode_getsize(&inp);
    int offset = blockNum * DISKIMG_SECTOR_SIZE;
    if (offset >= size) {
        return -1;
    }

    return (size - offset > DISKIMG_SECTOR_SIZE) ? DISKIMG_SECTOR_SIZE : (size - offset);
}

