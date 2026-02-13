# Timepad

A simple, elegant timer and stopwatch application built with C++23, SDL3, and ImGui.

## Features

- **Multiple Timers**: Create and manage multiple countdown timers
- **Stopwatch**: Track elapsed time with a stopwatch feature
- **Popout Windows**: Detach timers into separate windows for easy monitoring
- **Audio Alerts**: Get notified when timers complete
- **Modern UI**: Clean, intuitive interface using ImGui

## Dependencies

- CMake (>= 3.20)
- C++23 compatible compiler (GCC 13+ or Clang 15+)
- SDL3 (included as vendored dependency)
- ImGui (included as vendored dependency)

### Runtime Dependencies

- Wayland or X11 (for GUI)
- Audio system (ALSA, PulseAudio, PipeWire, or JACK)

## Building

```bash
# Configure the build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build

# The binary will be located at build/Timepad
```

## Installation

After building, you can run the application directly:

```bash
./build/Timepad
```

Or install it to your system (requires appropriate permissions):

```bash
sudo cmake --install build --prefix /usr/local
```

## Usage

1. Launch Timepad
2. Use the sidebar to switch between Timers and Stopwatches
3. Click "Create Timer" or "Create Stopwatch" to add new instances
4. Configure your timer/stopwatch and start tracking time
5. Use the popout feature to monitor timers in separate windows

## Project Structure

- `src/` - Main application source code
  - `ui/` - UI components and widgets
  - `main.cpp` - Application entry point
- `assets/` - Fonts and audio files
- `dependencies/` - Vendored third-party libraries

## License

See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
