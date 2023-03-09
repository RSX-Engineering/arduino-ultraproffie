<p style='text-align: left; font-size:66% '> ProffieOS v6.7x010  </p> 

# Arduino Plugin for [UltraProffie](https://ultraproffie.com)

## Installing

1. Prepare the [ProffieOS setup](https://pod.hubbe.net/proffieboard-setup.html).
2. Open Arduino, go to 'Preferences' and add the following Board Manager URL: https://ultraproffie.com/package_ultraproffie_index.json
3. Go to 'Boards Manager' and install the 'UltraProffie Line' package.
4. Download the [ProffieOSx source code](link) and open ProffieOS.ino.
5. Select the appropriate board in Ardunio. Make sure that the selected operating system is ProffieOSx.
6. Verify and upload as usual. 
7. Copy the [board files](https://ultraproffie.com/boardfiles), as well as any sound fonts and tracks you want, to the SD card. If your board is not equipped with an SD card, go to [ultraproffie.com](https://ultraproffie.com/), connect via USB and use 'Restore factory defaults' and 'Write font'.






## Credits

This core is based [Arduino Plugin for Proffieboard](https://github.com/profezzorn/arduino-proffieboard), which is based on the [Arduino STM32L4 Core](https://github.com/GrumpyOldPizza/arduino-STM32L4). 

---
<p style='text-align: right; font-size:66% '> FileVer. 1, 2023-01. (C)  RSX Engineering. License: GPL v3. </p> 