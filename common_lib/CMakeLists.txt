add_library(. STATIC
    i2c.c
    spi.c
    usart.c
    utils.c
    protocols/sd_card.c
    )

target_include_directories(. PUBLIC ..)
