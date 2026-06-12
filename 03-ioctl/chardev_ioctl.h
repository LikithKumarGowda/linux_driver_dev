#ifndef CHARDEV_IOCTL_H
#define CHARDEV_IOCTL_H

#include <linux/ioctl.h>

#define MAGIC_NUM 100

#define IOCTL_CLEAR     _IO(MAGIC_NUM, 0)
#define IOCTL_GET_LEN   _IOR(MAGIC_NUM, 1, int)
#define IOCTL_SET_MSG   _IOW(MAGIC_NUM, 2, char *)

#endif
