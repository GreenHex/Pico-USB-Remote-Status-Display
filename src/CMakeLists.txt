
aux_source_directory(. DIR_src_SRCS)

include_directories(../lib/Config)
include_directories(../lib/GUI)
include_directories(../lib/Infrared)
include_directories(../lib/LCD)
include_directories(../lib/Icm20948)
include_directories(../lib/OLED)

add_library(src ${DIR_src_SRCS})
target_link_libraries(src PUBLIC Config LCD Infrared Icm20948)
