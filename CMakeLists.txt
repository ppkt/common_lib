add_library(common_lib STATIC
    i2c.c
    spi.c
    usart.c
    utils.c
    protocols/sd_card.c
    )

target_include_directories(common_lib PUBLIC .)
