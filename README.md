# Kandey.fun - CS2 ESP Cheat

A simple CS2 ESP cheat with ImGui menu and box ESP around enemy players.

## Features
- ✅ Box ESP (corner-style) for enemy players
- ✅ ImGui menu for configuration
- ✅ Uses CS2-dumper SDK offsets (no hardcoded offsets!)
- ✅ Automatic offset updates supported

## Project Structure
```
newcs2/
├── core/
│   ├── dllmain.cpp          - DLL entry point
│   ├── entry.cpp            - Main thread & initialization
│   ├── patterns/
│   │   └── patterns.cpp     - Pattern scanning utilities
│   ├── offsets/
│   └── sdk/                 - CS2-dumper SDK files (organized by language)
│       ├── cpp/             - C++ headers with offsets
│       ├── csharp/          - C# files
│       ├── rust/            - Rust files
│       ├── zig/             - Zig files
│       └── json/            - JSON offset files
├── features/
│   └── esp/
│       ├── esp.cpp          - ESP rendering logic
│       ├── esp.h            - ESP header
│       └── chams.cpp        - Chams feature (placeholder)
├── main/
│   ├── gamestorage.cpp      - Game data storage
│   └── imgui/
│       ├── menu.cpp         - ImGui menu
│       ├── menu.h           - Menu header
│       └── imgui-master/    - ImGui library
└── AI_INSTRUCTIONS.txt      - Instructions for AI to auto-push to Git
```

## How It Works

### No Hardcoded Offsets!
The cheat uses the CS2-dumper SDK which provides offset files that are automatically generated. When CS2 updates, just run the dumper again and replace the SDK files - no code changes needed!

**Example from esp.cpp:**
```cpp
// Using SDK offsets instead of hardcoding
uintptr_t entityList = Read<uintptr_t>(clientDll + cs2_dumper::offsets::client_dll::dwEntityList);
ViewMatrix viewMatrix = Read<ViewMatrix>(clientDll + cs2_dumper::offsets::client_dll::dwViewMatrix);
```

### ESP System
- Reads entity list from game memory
- Filters out teammates and dead players
- Calculates 3D world positions
- Projects to 2D screen coordinates
- Draws corner-style boxes around enemies

### Menu System
- ImGui-based overlay menu
- Toggle ESP on/off
- Customize box color
- Adjust box thickness

## Building

1. Open Visual Studio
2. Create a new C++ DLL project
3. Add all files from the project structure
4. Include ImGui library
5. Link against d3d11.lib
6. Compile as x64 DLL

## Injecting

Use any DLL injector to inject the compiled DLL into cs2.exe. Press `INSERT` to toggle the menu.

## Updating Offsets

1. Download latest CS2-dumper from: https://github.com/a2x/cs2-dumper
2. Run the dumper against CS2
3. Copy the output files to `core/sdk/`
4. No code changes needed - offsets are automatically loaded!

## Safety Features

- Exception handling on memory reads
- Validity checks on all pointers
- Team filtering (only shows enemies)
- Health validation (only shows alive players)

## Credits

- CS2-dumper for offset generation
- ImGui for menu system
- Velocity CS2 for ESP implementation reference

---

**Disclaimer:** This is for educational purposes only. Use at your own risk.
