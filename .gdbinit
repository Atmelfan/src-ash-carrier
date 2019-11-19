# get around CLion/QtCreator not supporting target extended-remote
define target remote
  target extended-remote $arg0
end

# optional: prevent gdb asking for confirmation
# when invoking the run command in gdb
set confirm off

# Set timeout to a higher value
set remotetimeout 100000

# Print the Black Magic Probe firmware version
monitor version

# Optional: Enable target power
#monitor tpwr enable

# Optional: Enable system Reset on connect
#monitor connect_srst enable

# Scan for targets using SWD protocol
monitor swdp_scan
# Alternative: Use JTAG protocol to talk to the target
#monitor jtag_scan

# Attach to the first device found
attach 1

# Erase and load the provided binary to flash
load

# Compare the loaded sections to the ones in the provided binary
compare-sections
# Reset and disconnect from the target
#kill
run
