
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (pathname == NULL || pathname[0] != '/') {
        return -1;
    }
    if (pathname[1] == '\0') {
        return ROOT_INUMBER;
    }

    char path_copy[strlen(pathname) + 1];
    strcpy(path_copy, pathname + 1);
    char *component = strtok(path_copy, "/");
    int inumber = ROOT_INUMBER;

    while (component != NULL) {
        struct inode inp;
        if (inode_iget(fs, inumber, &inp) != 0 || (inp.i_mode & IFMT) != IFDIR) {
            return -1;
        }
        struct direntv6 dirEnt;
        if (directory_findname(fs, component, inumber, &dirEnt) != 0) {
            return -1;
        }
        inumber = dirEnt.d_inumber;
        component = strtok(NULL, "/");
    }

    return inumber;
}
