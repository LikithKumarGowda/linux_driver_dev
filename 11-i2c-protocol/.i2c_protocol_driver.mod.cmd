savedcmd_i2c_protocol_driver.mod := printf '%s\n'   i2c_protocol_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > i2c_protocol_driver.mod
