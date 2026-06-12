# WHAT'S THIS?
This is a BPSK encoder and decoder written in plain C.
Generates a BPSK signal at 1000Hz and decodes it from a .wav file.

## Settings
Set the bitrate at `int bitrate = 10`, but you need to do that in `decoder.c`, `encoder.c` and at `func.h - bpsk_encoding()`.  
Set filename at `encoder.c - write_wav()` and at `decoder.c - read_wav()`  
Set text at `encoder.c - char text[] = "Here";`

## Instructions
Compiling on Windows:
```
$ git clone https://github.com/klaw630/bpsk.git
$ cd bpsk
$ clang decoder.c -o decoder.exe
$ clang encoder.c -o encoder.exe
```
Compiling on linux:
TODO

## Running
CMD: `encoder.exe` or `decoder.exe`
PowerShell: `.\encoder.exe` or `.\decoder.exe`
Git bash or any other `./encoder.exe` or `./decoder.exe`

## Results
At 10bps, I got a errorless decode at just 2.5dB!
At 100bps, I got it at 5dB.
Both with 86 characters long text.

This repo is made for entertainment purposes only.
