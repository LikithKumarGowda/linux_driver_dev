#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "chardev_ioctl.h"

int main()
{
    int fd, len;

    fd = open("/dev/ioctldev", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Setting message via ioctl...\n");
    ioctl(fd, IOCTL_SET_MSG, "Hello via IOCTL!");

    ioctl(fd, IOCTL_GET_LEN, &len);
    printf("Buffer length: %d\n", len);

    char buf[1024] = {0};
    read(fd, buf, sizeof(buf));
    printf("Read back: %s\n", buf);

    printf("Clearing buffer...\n");
    ioctl(fd, IOCTL_CLEAR);

    ioctl(fd, IOCTL_GET_LEN, &len);
    printf("Buffer length after clear: %d\n", len);

    close(fd);
    return 0;
}
