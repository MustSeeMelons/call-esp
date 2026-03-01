# Call ESP

A LILYGO T-Call project for automating pesky phone calls to open gates.

Has a SPI connected 8x8 LED matrix for text and animations.

Listens to specific BLE ad messages and performs the phone calls.

Runs off USB power. Perfect for car use. 

Button toggles between two animations. 

The BLE button trigger lives here: [nrf-gate-beacon](https://github.com/MustSeeMelons/nrf-gate-beacon)

# Text

`letters.c` defiens all the letters and text. Extra symbols can be added as well.

# Anims
Can be done in the React project in the `frames` directory, then each frame bytes can be copied and setup in the `anims.c` file following existing samples.

# Phone numbers:
- Rename `include/defs_sample.h` as `include/defs.h` and place the phone numbers into `PHONE_FIRST` and `PHONE_SECOND` 
- Update `cmd_call_one` and `cmd_call_other` `cmd_len` property.

# Pics

![Alt text](/parts.jpg)

Yes, one of the connections was wrong so I had to jump it. The cad files are fixed. I did not bother ordering another batch of PCB's for such a minor issue.

![Alt text](/anim.jpg)

# Extras

The SIM800L code should be real world tested for errors, for example if the sim card credit is 0 or it has expired.