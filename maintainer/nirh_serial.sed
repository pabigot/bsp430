# sed script to update for interface change post 20130624 release
# Use: sed -i -r -f nirh_serial.sed
# Find: git ls-tree HEAD -r -z --name-only | xargs -0 ${Use}
s/([sS]etReset)_ni\b/\1_rh/g
s/([sS]etHold)_ni\b/\1_rh/g
s/([wW]akeupTransmit)_ni\b/\1_rh/g
s/((uart|UART)[rtRT]x(Byte|Data|ASCIIZ))_ni\b/\1_rh/g
s/((spi|SPI)TxRx)_ni\b/\1_rh/g
s/((i2c|I2C)[rtRT]xData)_ni\b/\1_rh/g
s/((i2cS|I2Cs)etAddresses)_ni\b/\1_rh/g
s/((m25p)[a-zA-Z]*)_ni\b/\1_rh/g
