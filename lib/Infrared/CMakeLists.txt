
aux_source_directory(. DIR_Infrared_SRCS)

include_directories(../Config)


add_library(Infrared ${DIR_Infrared_SRCS})
target_link_libraries(Infrared PUBLIC Config)
