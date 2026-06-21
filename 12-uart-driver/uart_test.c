#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

int configure_uart(int fd)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr failed");
        return -1;
    }

    /* Set baud rate to 115200 */
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    /* 8N1 - 8 data bits, no parity, 1 stop bit */
    tty.c_cflag &= ~PARENB;   /* no parity */
    tty.c_cflag &= ~CSTOPB;   /* 1 stop bit */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;       /* 8 data bits */
    tty.c_cflag |= CLOCAL | CREAD;  /* enable receiver, local mode */

    /* Raw input/output - no special processing */
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    /* Add read timeout so read() doesnt block forever */
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 10; /* 1 second timeout (in deciseconds) */

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");
        return -1;
    }

    printf("UART configured: 115200 baud, 8N1\n");
    return 0;
}

int main()
{
    int fd;
    char tx_buf[] = "Hello UART from BeagleBone!";
    char rx_buf[128] = {0};
    int n;

    fd = open("/dev/ttyAMA1", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open /dev/ttyAMA1");
        return 1;
    }

    if (configure_uart(fd) < 0) {
        close(fd);
        return 1;
    }

    printf("Writing: %s\n", tx_buf);
    n = write(fd, tx_buf, strlen(tx_buf));
    printf("Wrote %d bytes\n", n);

    /* Give it a moment then try to read back */
    usleep(100000);

    n = read(fd, rx_buf, sizeof(rx_buf) - 1);
    if (n > 0) {
        rx_buf[n] = '\0';
        printf("Read %d bytes: %s\n", n, rx_buf);
    } else {
        printf("No data read back (expected without physical loopback wiring)\n");
    }

    close(fd);
    return 0;
}
