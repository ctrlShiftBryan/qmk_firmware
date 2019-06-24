# ProjectKeyboard Alice PCB

ProjectKeyboard Alice Keyboard Replacement PCB

Keyboard Maintainer: onefiftynine 

Hardware Supported: STM32F072CBT6  
Make example for this keyboard (after setting up your build environment):

    make projectkb/alice:default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

```
# make the keymap
make projectkb/alice:ctrlshiftba

# find the device
dfu-util --list

# dfu-util to flash
dfu-util -a 0 -d 0483:df11 -s 0x8000000:leave -D /Users/barendt/QMK/qmk_firmware/projectkb_alice_ctrlshiftba.bin
```
