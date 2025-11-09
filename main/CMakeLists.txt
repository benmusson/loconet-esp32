set(srcs "m18_bms_esp32.c")

idf_component_register(SRCS "${srcs}"
        INCLUDE_DIRS "."
        PRIV_REQUIRES libm18)
