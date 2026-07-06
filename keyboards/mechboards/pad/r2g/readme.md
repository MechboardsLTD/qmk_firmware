# Pad R2G

A ready 2 go macro pad PCB with VIK support.

* Keyboard Maintainer: [dasky](https://github.com/daskygit)
* Hardware Availability: https://mechboards.co.uk

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb mechboards/pad/r2g -km default

Flashing example for this keyboard:

    qmk flash -kb mechboards/pad/r2g -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the top left key when plugging in USB.
* **Boot button**: Hold down the small button next to the USB when plugging in the USB.
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
