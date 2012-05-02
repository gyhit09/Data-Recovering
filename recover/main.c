#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include "ext2.h"

int main(int argc, char * argv[]) {

    char path[500];
    printf("Input the path of part:\n");
    scanf("%s", path);
    init(path);
    scan_node();
    mdev_close();

    return 0;

}