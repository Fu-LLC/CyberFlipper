
# CyberFlipper Infrared Database

This repository contains IR files and protocol information for use with the CyberFlipper project. All referenced products, brands, and resources are cited for informational purposes only. No explicit creator branding or donation links are present. For full legal details and contributor credits, see the repository's LICENSE and NOTICE files.

---

## Protocol Info
This info was gathered from respective locations in firmware comments and open source documentation. For details, see:
- [Kaseikyo protocol description](https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/ir_Kaseikyo.hpp)
- [NEC protocol description](https://radioparty.ru/manuals/encyclopedia/213-ircontrol?start=1)

### RC5 protocol description
https://www.mikrocontroller.net/articles/IRMP_-_english#RC5_.2B_RC5X
*                                       Manchester/biphase
*                                           Modulation
*
*                              888/1776 - bit (x2 for toggle bit)
*
`                           __  ____    __  __  __  __  __  __  __  __`<br>
`                         __  __    ____  __  __  __  __  __  __  __  _`<br>
*                         | 1 | 1 | 0 |      ...      |      ...      |
*                           s  si   T   address (MSB)   command (MSB)
*
*    Note: manchester starts from space timing, so it have to be handled properly
*    s - start bit (always 1)
*    si - RC5: start bit (always 1), RC5X - 7-th bit of address (in our case always 0)
*    T - toggle bit, change it's value every button press
*    address - 5 bit
*    command - 6/7 bit

### RC6 protocol description
https://www.mikrocontroller.net/articles/IRMP_-_english#RC6_.2B_RC6A
*      Preamble                       Manchester/biphase                       Silence
*     mark/space                          Modulation
*
*    2666     889        444/888 - bit (x2 for toggle bit)                       2666
*
`  ________         __    __  __  __    ____  __  __  __  __  __  __  __  __`<br>
` _        _________  ____  __  __  ____    __  __  __  __  __  __  __  __  _______________`<br>
*                   | 1 | 0 | 0 | 0 |   0   |      ...      |      ...      |             |
*                     s  m2  m1  m0     T     address (MSB)   command (MSB)
*
*    s - start bit (always 1)
*    m0-2 - mode (000 for RC6)
*    T - toggle bit, twice longer
*    address - 8 bit
*    command - 8 bit

### SAMSUNG32 protocol description
https://www.mikrocontroller.net/articles/IRMP_-_english#SAMSUNG <br>
*  Preamble   Preamble     Pulse Distance/Width        Pause       Preamble   Preamble  Bit1  Stop
*    mark      space           Modulation                           repeat     repeat          bit
*                                                                    mark       space
*
*     4500      4500        32 bit + stop bit       40000/100000     4500       4500
`  __________          _  _ _  _  _  _ _ _  _  _ _                ___________            _    _`<br>
` _          __________ __ _ __ __ __ _ _ __ __ _ ________________           ____________ ____ ___`

### Sony SIRC protocol description
https://www.sbprojects.net/knowledge/ir/sirc.php <br>
http://picprojects.org.uk/
*      Preamble  Preamble     Pulse Width Modulation           Pause             Entirely repeat
*        mark     space                                     up to period             message..
*
*        2400      600      12/15/20 bits (600,1200)         ...45000          2400      600
`     __________          _ _ _ _  _  _  _ _ _  _  _ _ _                    __________          _ _`<br>
` ____          __________ _ _ _ __ __ __ _ _ __ __ _ _ ____________________          __________ _`
*                        |    command    |   address    |
*                 SIRC   |     7b LSB    |    5b LSB    |
*                 SIRC15 |     7b LSB    |    8b LSB    |
*                 SIRC20 |     7b LSB    |    13b LSB   |
*
* No way to determine either next message is repeat or not,
* so recognize only fact message received. Sony remotes always send at least 3 messages.
* Assume 8 last extended bits for SIRC20 are address bits.

-----

## Donation Information

Nothing is ever expected for the hoarding of digital files, creations I have made, or the people I may have helped.

## Ordering from Lab401? [USE THIS LINK FOR 5% OFF!](https://lab401.com/r?id=vsmgoc) (Or code `UberGuidoZ` at checkout.)

I've had so many asking for me to add this.<br>
![Flipper_Blush](https://user-images.githubusercontent.com/57457139/183561666-4424a3cc-679b-4016-a368-24f7e7ad0a88.jpg) ![Flipper_Love](https://user-images.githubusercontent.com/57457139/183561692-381d37bd-264f-4c88-8877-e58d60d9be6e.jpg)

**BTC**: `3AWgaL3FxquakP15ZVDxr8q8xVTc5Q75dS`<br>
**BCH**: `17nWCvf2YPMZ3F3H1seX8T149Z9E3BMKXk`<br>
**ETH**: `0x0f0003fCB0bD9355Ad7B124c30b9F3D860D5E191`<br>
**LTC**: `M8Ujk52U27bkm1ksiWUyteL8b3rRQVMke2`<br>
**PayPal**: `uberguidoz@gmail.com`

So, here it is. All donations of *any* size are humbly appreciated.<br>
![Flipper_Clap](https://user-images.githubusercontent.com/57457139/183561789-2e853ede-8ef7-41e8-a67c-716225177e5d.jpg) ![Flipper_OMG](https://user-images.githubusercontent.com/57457139/183561787-e21bdc1e-b316-4e67-b327-5129503d0313.jpg)

Donations will be used for hardware (and maybe caffeine) to further testing!<br>
![UberGuidoZ](https://cdn.discordapp.com/emojis/1000632669622767686.gif)
