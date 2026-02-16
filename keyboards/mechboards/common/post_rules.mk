ifeq ($(strip $(OLED_ENABLE)), yes)
	SRC += keyboards/mechboards/common/display_oled.c
endif
ifeq ($(strip $(QUANTUM_PAINTER_ENABLE)), yes)
	SRC += keyboards/mechboards/common/display_lcd.c
	SRC += keyboards/mechboards/common/qp_font/pixellari18.qff.c
	SRC += keyboards/mechboards/common/qp_font/pixellari24.qff.c
endif
