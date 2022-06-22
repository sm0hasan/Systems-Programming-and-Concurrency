#include <stdio.h>
#include <string.h> // memset
#include <sys/shm.h> // shmget, shmat, shmdt, shmctl
#include <sys/wait.h> // wait
#include <unistd.h> // fork

int main( int argc, char *argv[] ) {

    int shmid = shmget(IPC_PRIVATE, 32, IPC_CREAT | 0600);
    void *mem;
    // mem = shmat(shmid, NULL, 0);

    switch(fork()) {
    case -1:
        perror(NULL);
        return 1;

    case 0: // child
        mem = shmat(shmid, NULL, 0);
        memset(mem, 0, 32);
        sprintf(mem, "Hello parent");
        // shmdt(mem);
        break;

    default: // parent
        wait(NULL);
        mem = shmat(shmid, NULL, 0);
        printf("Message from child proc: %s\n", (char *)mem);
        shmdt(mem);
        shmctl(shmid, IPC_RMID, NULL);
        break;

    }

    return 0;
}