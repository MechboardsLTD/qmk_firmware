QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += st7735_spi
CONSOLE_ENABLE = yes
WPM_ENABLE = yes

# SRC += gfx/mb.qgf.c
SRC += display/display.c
SRC += display/screens.c
SRC += display/gfx/pixellari14.qff.c
SRC += display/gfx/pixellari18.qff.c
SRC += display/gfx/pixellari24.qff.c