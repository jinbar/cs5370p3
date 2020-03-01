/* any write in the proctected area should be killed  */
#include "include/types.h"
#include "include/stat.h"
#include "user/user.h"
#include "kernel/vm.c"


#define PGSIZE 4096
int main() {
    const uint numframes = 100;
    // sbrk(numframes * PGSIZE);
    int frames[numframes];

    if (dump_allocated(frames, numframes) != 0) {
        printf("Error: dump_allocated return non-zero value");
        printf("HEHEHEHE\n");
        printf("TEST FAILED\n");
        exit();
    }

    for (int i = 0; i< numframes; i++) {
        for (int j = 0; j < numframes; j++) {
            if (i != j && (frames[j] == frames[i] 
                || frames[j] == frames[i] + PGSIZE || frames[j] == frames[i] - PGSIZE)) {
                    printf("ASDFASDf\n");
                printf("TEST FAILED\n");
                exit();
            }
        }
    } 

    printf("TEST PASSED\n");
    exit();
}
