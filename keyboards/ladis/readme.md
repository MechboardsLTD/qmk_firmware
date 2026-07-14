# Ladispad

Ladispad is a custom-made 11 key macropad with 2 rotary encoders designed for the simplicity and the ease of assembly for non technical users. Macropads are not a new thing. Browsing the internet will lead to to dozens of open-source macropads that let you print your own PCB and assemble it at home, but most of these macropads use matrixes, which will force you to solder diodes adding a little complexity to the process.

Ladispad uses a direct pin approach. That means you don't need to solder diodes. Additionally, I managed to add 2 rotary encoders and a fully customizable OLED screen, which can be omitted if you choose.

Ladispad uses a Pro Micro as its main controller. It's very affordable, and there are many variants with different USB ports and sizes. I designed the PCB so it will be compatible which whatever you choose.

* Hardware Supported: *Promicro, OLED, Encoders*

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb ladis -km default

Flashing example for this keyboard:

    qmk flash -kb ladis -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (top left key) and plug in the keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
