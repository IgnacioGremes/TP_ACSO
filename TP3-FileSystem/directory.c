#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  struct inode inp;
  if (inode_iget(fs, dirinumber, &inp) != 0 || !(inp.i_mode & IALLOC) || (inp.i_mode & IFMT) != IFDIR) {
      return -1;
  }

  int size = inode_getsize(&inp);
  int num_blocks = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
  char buf[DISKIMG_SECTOR_SIZE];

  for (int bno = 0; bno < num_blocks; bno++) {
      int bytes = file_getblock(fs, dirinumber, bno, buf);
      if (bytes < 0) {
          return -1;
      }
      int entries = bytes / sizeof(struct direntv6);
      struct direntv6 *entry = (struct direntv6 *)buf;
      for (int i = 0; i < entries; i++) {
          if (strncmp(entry[i].d_name, name, 14) == 0) {
              *dirEnt = entry[i];
              return 0;
          }
      }
  }

  return -1;
}
