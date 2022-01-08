# Arduino-VUMeter-Cpp

## Arduino VU meter written in c++
### Only for linux with PulseAudio

```libpulse``` is required to compile the code.
Tested on Arch Linux + KDE plasma 5

To build, change the serial port and audio device in the code first.

List Audio devices using :
```bash
$ LANG=C pactl list | grep -A2 'Source #' | grep 'Name: ' | cut -d" " -f2
```
To build and run :
```
./build.sh && ./bin/main
```
