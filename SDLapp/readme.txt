This is another Android Dosbox port.
Made without any optimizations and testing, so may crash/hang/have bugs.
I've just compuled the original Dosbox code with my own GCC toolkit 
(with google gcc patches of cause), and my own SDL port. Made just a few
minor changes in Dosbox sources. Would publish them later.

Mouse is emulated badly. Keyboard emulation is far from perfect. No sound/midi.
To call on-screen keyboard press and hold "menu" key.
Compiled for Android 1.6+, tested only on HTC Desire with Android 2.1

On first run default config is created in <SD card path>/SDL/.dosbox/
Dynamic core is not tested, but may work. 

(c) mamaich@pisem.net
