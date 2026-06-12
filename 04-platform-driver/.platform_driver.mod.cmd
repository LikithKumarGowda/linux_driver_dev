savedcmd_platform_driver.mod := printf '%s\n'   platform_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > platform_driver.mod
