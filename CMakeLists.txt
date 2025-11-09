cmake_minimum_required(VERSION 3.5)

set(SOURCES
        "src/loconet/src/loconet.c"
        "src/loconet/src/loconet_message.c"
        "src/loconet_esp32.c"
        )

set(INCLUDES
        "src/loconet/src"
        "src"
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDES}
    REQUIRES driver
)