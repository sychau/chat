#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char hostname[256];

    // Get the hostname
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("Hostname: %s\n", hostname);
    } else {
        perror("gethostname");
        return 1;
    }

    return 0;
}