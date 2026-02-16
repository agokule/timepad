# Timepad

Everything was going mostly well after I started dual booting Windows and Arch linux, except that I wasn't able to find a good timer app which had a sort of picture-in-picture mode, just like the Windows timer app.

So I made this (no that is literally the reason why I made this app).

## Installation

You can use AUR packages `timepad` or `timepad-git` to install this app with your favorite AUR helper.

## Build from Source

You can build this from source using the following commands:

```bash
git clone https://github.com/agokule/timepad.git --recurse-submodules
cd timepad
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug # or Release
cmake --build build # if you are using make, you can add "-- -j" to make it compile faster
```

Now the binary will be in `./build/Timepad`.

## Screenshots and Videos

<img width="959" height="459" alt="image" src="https://github.com/user-attachments/assets/49374b52-d15d-42d0-a9f4-50d7a7564bb7" />

[2026-02-16 15-40-58.webm](https://github.com/user-attachments/assets/45b8f853-91c2-41f3-b1df-0c5109e58484)

## Credits

[Timer Icon by Freepik on Flaticon](https://www.flaticon.com/free-icon/stopwatch_2055568?related_id=2055768&origin=search)

[Timer Sound Effect](https://pixabay.com/sound-effects/household-kitchen-timer-87485/)

