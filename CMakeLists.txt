add_library(common_lib STATIC
    i2c.c
    usart.c
    utils.c
    )

target_include_directories(common_lib PUBLIC .)
