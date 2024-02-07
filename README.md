# 1989 Nintendo Game Boy Emulator
## Overview
This is an emulator for the [1989 Nintendo Game Boy](https://en.wikipedia.org/wiki/Game_Boy), written in C++ using SDL. It's accurate enough to run the one GB game I still have a cartridge for, Tetris, but does not support audio (which, for Tetris at least, is admittedly a massive loss) or memory bank switching, which extends the 16-bit address space to use additional memory supplied on the cartridge. As such, not all games are runnable.

![Tetris gameplay](https://www.wjgrace.co.uk/images/gb_thumbnail.gif)

*My emulator playing Tetris*

My primary resource whilst programming this emulator was [Pan Docs](https://gbdev.io/pandocs/), which describes the hardware in detail, and [this filterable CPU instruction code table](https://izik1.github.io/gbops/). This was a pretty fun project which demonstrated the importance of early and extensive testing. Even implementing seemingly simple opcodes introduced errors that would have been difficult to detect without unit tests.

There are a few things I'd change if I were to do it again - for instance, using separate variables for the named areas of memory would have been more expressive (and less prone to errors) than a single array as used in the MemoryMap class. Similarly, using interface classes would have reduced the coupling between components.

## Dependencies and Compilation

The only dependency is SDL, which is used for window creation and rendering the emulator's output.

Here is a sample GCC compilation command (of course, include paths may vary), which executes with no warnings on my computer:
```
g++ src\*.cpp -o "gb-emu.exe" -W -Wall -Wextra -pedantic -I "C:\SDL-release-2.26.4\include" -I "C:\w64devkit\include" "SDL2.dll" -std=c++20 -O3 -DNDEBUG
```

## Usage

Command line interface (parameters may be provided in any order):
```
.\gb-emu.exe 
    -i [PATH_TO_INPUT_ROM] (the path to the game file)
    OPTIONAL: -b [PATH_TO_BOOT_ROM] (the path to a boot program, if not provided, boot is simulated)
    OPTIONAL: -v (display the output of the Game Boy's serial port at the command line)
    OPTIONAL: -t (runs unit tests, ignoring all other arguments)
```
To run the tests, put these JSONs (which include random testing data for the opcodes) in a folder named 'test' within the same directory as the executable: https://github.com/adtennant/sm83-test-data/tree/master/cpu_tests/v1.

As with all emulators, **this should not be used for playing games that have been acquired illegally**. I have a physical copy of Tetris, from which I extracted a personal copy of the ROM (for my own private study) using a special adapter. Distributing or downloading games is highly likely to be copyright infringement, so please do not do it.

Of course, freely released 'homebrew' games are generally fair game, as are games you write yourself! A fun project for the future might be to use [GBDK](https://github.com/gbdk-2020/gbdk-2020) to make a small game of some description, though this is likely a non-trivial task.
