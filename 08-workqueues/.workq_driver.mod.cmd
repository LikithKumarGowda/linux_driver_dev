savedcmd_workq_driver.mod := printf '%s\n'   workq_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > workq_driver.mod
