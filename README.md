Description:
============

This library permits to produce DTMF ( https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling ) tones using Alsa lib and a sound card under Linux and can encode standard file ( text or binary data) in a DTMF tones sequence.

Trivia:
=======

I intended to use it for radio hacks / experiments. Then I watched Bandersnatch ( https://en.wikipedia.org/wiki/Black_Mirror:_Bandersnatch ) and I felt the necessity to try, in 2019, the experience ( read "the pain") of saving a file on audio cassette. So I added a command line tool based able to convert a file (binary or text ) in a sequence of DTMF tones  There are 16 DTMF tones, so you can covert a byte in a sequence of two DTMF tones, one encoding the four most significative bits, the other the less significative. For now, the speed is 4 tones for second, so 2 bytes for second. Definitely it's slower than a C64 cassette player ( 300 byte per second, if I remember), but ITU specs show that the speed should be improved drastically, maybe in the future I'll do some changes ( suggestions are welcome ), for now I think it's awesome, so 80s (maybe 70s) ! 

DISCLAIMER:
===========

This code has been posted for information and educational purposes. The intrusion in systems and services without the owner's authorisation is illegal. The misuse of the information and the software in this repo can result in criminal charges brought against the perpetrator of the crime. Any actions and or activities related to the material contained within this Git Repository is solely your responsibility. The author will not be held responsible in the event any criminal charges be brought against any individuals misusing the information or the software in this website to break the law.

Prerequisites:
==============

The program is intended to be used in a Linux environment and it is tested on various Linux distributions:

- Ubuntu 18.04.1 LTS

using, as compiler, one in this list:

- gcc version 7.3.0 (Ubuntu 7.3.0-27ubuntu1~18.04) 

This program needs Alsa library.

Installation:
=============

- launch the configure script:
  ./configure
- Compile the program:
  make
- Install the program and the man page:
  sudo make install

Instructions:
=============

- Produce a sequence of DTMF tones:
   ./src/dtmf -s "1 1 1"
  spaces are pauses.
- Convert a file (binary or text) to a file DTMF encoded:
   ./src/dtmf -b ./test/dtmf.bintest -o./xxx
  dtmf.bintest is the original, xxx the destination.
- Decode the previous file:
  ./src/dtmf -t./xxx  -o./dtmf.bintest.again
- To "play" an encoded file, you can pass it on stdin:
   cat xxx | ./src/dtmf
  maybe you want save it on a cassette recoder! :-)

Documentation:
==============

Man pages and examples are present in the distribution.
