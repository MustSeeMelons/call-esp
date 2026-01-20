# Call ESP

A LILYGO T-Call project for automating pesky phone call to open gates.

Has a SPI connected 8x8 LED matrix for text and animations.

Listens to specific BLE ad messages and performs the phone calls.

Runs off USB power. Perfect for car use.

**TODO:** Link the button repo which uses a NRF52840 to broadcast ads on button press. That still needs some work.
**TODO:** Add some pics.

# Text

`letters.c` defiens all the letters and text. Extra symbols can be added as well.

# Anims
Can be done in the React project in the `frames` directory, then each frames bytes can be copied and setup in the `anims.c` file following existing samples.

# Phone numbers:
- Rename `include/defs_sample.h` as `include/defs.h` and place the phone numbers into `PHONE_FIRST` and `PHONE_SECOND` 
- Update `cmd_call_one` and `cmd_call_other` `cmd_len` property.

