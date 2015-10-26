# Moons

![Small screenshot of Moons in action](/img/moons_sm.png)

*Moons* is an isorhythmic circular sequencer. It is written using a combination of C and C++.
All visuals are created using OpenGl; All the sounds are snythesized in realtime
using [Sporth](https://paulbatchelor.github.io/proj/sporth.html), [Soundpipe](https://paulbatchelor.github.io/proj/soundpipe.html), and RTaudio.

## Compilation/Installation

The latest version of Moons can be found on [github](https://www.github.com/PaulBatchelor/Moons).

Moons requires libsporth ([github](http://www.github.com/paulbatchelor/sporth.git)) 
and the **dev branch** of libsoundpipe 
([github](http://www.github.com/paulbatchelor/soundpipe.git)). On Linux, JACK and
the development libraries will also need to be installed.

After these have been installed, you can then compile Moons with:

make SR=44100

Where SR is the samplerate your machine is running at (which is typically 44100 or
48000). 

If this is compiled successfully, you should now have a binary called "moons" that 
you can run with:

./moons

### Using soundpipe and sporth locally with moons

If you do not want to install soundpipe and sporth on your machine, or do not have
the right admin permissions to do so (like a CCRMA machine), you can place libsporth.a
and libsoundpipe.a into the project folder in a directory you create called "libs".
The header files can be placed in tthe top level drectory. From soundpipe, you will
need "h/soundpipe.h". From Sporth, you will need "tmp.h", but it must be renamed
to "sporth.h" (if tmp.h does not exist, run "make tmp.h" in the Sporth source).

After you have done all that, you can proceed with compiling Moons the usual way.

## Usage

When Moons first starts up, you will be faced with a black screen with background
music playig. Clicking somewhere will create a satellite that orbits an 
imaginary point at the center of the screen. Every time a satellite makes a full
rotation, it will create a bell-like tone. 

The farther away from the center you click, the larger the radius will be, and 
therefore the time it takes to make a full rotation. Where on the circle you decide
to click will determine the note values. Currently, there are 7 distinct notes to choose
from, each color coded.

### Keyboard Commands

'q' - cleanly quit

'u' - remove last moon created

'space' - starts/stops the sound (the reverb still stays on)

'1-4' - change the scale/chord

{{FOOTER}}
