savedcmd_ldisc_driver.mod := printf '%s\n'   ldisc_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > ldisc_driver.mod
