savedcmd_timer_driver.mod := printf '%s\n'   timer_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > timer_driver.mod
