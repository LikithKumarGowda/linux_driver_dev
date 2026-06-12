savedcmd_ioctl_driver.mod := printf '%s\n'   ioctl_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > ioctl_driver.mod
