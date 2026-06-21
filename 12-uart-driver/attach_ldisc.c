#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define N_MYPROTO 25

int main(int argc, char *argv[])
{
    int fd, ldisc = N_MYPROTO;

    if (argc != 2) {
        printf("Usage: %s /dev/ttyAMAx\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    if (ioctl(fd, TIOCSETD, &ldisc) < 0) {
        perror("TIOCSETD failed");
        close(fd);
        return 1;
    }

    printf("Line discipline %d attached to %s\n", ldisc, argv[1]);
    printf("Press Enter to detach and exit...\n");
    getchar();

    /* Detach by switching back to N_TTY (0) */
    ldisc = 0;
    ioctl(fd, TIOCSETD, &ldisc);
    close(fd);
    return 0;
}
