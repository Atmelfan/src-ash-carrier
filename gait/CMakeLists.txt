add_library(gait gait.c gait.h gait_dts_const.h)

##Add gait FDT blob...
include(../fdt/fdt.cmake)
#add_fdt_target(gait_dts gaits.dts)
target_link_libraries(gait)
