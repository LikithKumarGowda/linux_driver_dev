savedcmd_sysfs_driver.mod := printf '%s\n'   sysfs_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > sysfs_driver.mod
