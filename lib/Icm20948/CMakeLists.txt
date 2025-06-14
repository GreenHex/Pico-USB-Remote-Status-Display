
aux_source_directory(. DIR_Icm20948_SRCS)

include_directories(../Config)

add_library(Icm20948 ${DIR_Icm20948_SRCS})
target_link_libraries(Icm20948 PUBLIC Config)
