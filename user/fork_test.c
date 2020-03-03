#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int main(int argc, char *argv[]) {
    uchar *p = 0;
    for (int i = 0; i < 4; i++) {
        printf(1, "Old %p\n", (uchar) *p);
    }
    mprotect(0, 1);
    int result = fork();
    if(result != 0) {
       wait();
       printf(1,"-Starting Parent\n");
       munprotect(0,1); // Parent reset to unproected AFTER fork
    } else {
       printf(1,"-Starting Child\n");
    }
    *p = (uchar)0xFF;
    printf(1, "New %p\n", (uchar) *p);
    exit();
    return 0;
}