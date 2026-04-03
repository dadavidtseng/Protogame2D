<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![DirectX](https://img.shields.io/badge/DirectX-11-107C10?style=for-the-badge&logo=xbox&logoColor=white)
![FMOD](https://img.shields.io/badge/FMOD-000000?style=for-the-badge&logo=fmod&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
[![Apache 2.0 License][license-shield]][license-url]

<!-- PROJECT TITLE -->
<div align="center">
  <h1>Protogame2D</h1>
  <p>A minimal 2D game template built on the Daemon Engine for rapid prototyping</p>
</div>

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [How to Install](#how-to-install)
- [How to Use](#how-to-use)
- [Project Structure](#project-structure)
- [License](#license)
- [Contact](#contact)

## Overview

Protogame2D is a starter template for building 2D games on the custom [Daemon Engine](https://github.com/dadavidtseng/Engine). It provides a clean, minimal scaffold with an application lifecycle, a two-state game state machine (Attract → Game), an orthographic camera, event-driven state transitions with audio feedback, and built-in debug tools (FPS counter, pause, slow-mo, single-frame step).

Designed as a boilerplate that can be cloned and extended into a full game project. The V8 scripting subsystem is disabled by default via `EngineBuildPreferences.hpp` to keep the build lightweight.

## Features

- **Game state machine** — Event-driven Attract ↔ Game transitions with `OnGameStateChanged` callbacks and audio cues
- **2D orthographic rendering** — Screen-space camera with `VertexList_PCU` immediate-mode drawing (disc, line segments)
- **Debug tools** — Real-time FPS/time/scale HUD, pause (P), single-frame step (O), slow-mo at 0.1× (T hold)
- **Window management** — Fullscreen stretch toggle (R), dynamic client dimension tracking
- **Structured logging** — `DAEMON_LOG` macro with verbosity levels for lifecycle tracing

## How to Install

### Prerequisites

- Visual Studio 2022 (or 2019) with C++ desktop development workload
- Windows 10/11 (x64)
- DirectX 11 compatible GPU
- [Daemon Engine](https://github.com/dadavidtseng/Engine) cloned as a sibling directory

### Build

```bash
# Clone both repos side by side
git clone https://github.com/dadavidtseng/Engine.git
git clone https://github.com/dadavidtseng/Protogame2D.git

# Directory layout should be:
# ├── Engine/
# └── Protogame2D/
```

1. Open `Protogame2D.sln` in Visual Studio
2. Set configuration to `Debug | x64`
3. Build the solution (the Engine project is referenced automatically)
4. The executable is deployed to `Run/` via post-build event

## How to Use

### Controls

| Action | Key |
|--------|-----|
| Start game | Space |
| Return to attract / Quit | ESC |
| Fullscreen toggle | R |
| Restart game | F8 |

### Debug Controls

| Action | Key |
|--------|-----|
| Pause / Unpause | P |
| Step single frame | O |
| Slow-mo (0.1×) | T (hold) |

### Running

Launch `Run/Protogame2D_Debug_x64.exe` from the `Run/` directory (working directory must be `Run/` for asset loading).

## Project Structure

```
Protogame2D/
├── Code/Game/
│   ├── Framework/                 # Application framework
│   │   ├── Main_Windows.cpp       # WinMain entry point
│   │   ├── App                    # Application lifecycle (Startup → RunMainLoop → Shutdown)
│   │   └── GameCommon             # Global pointers, debug draw helpers, GAME_SAFE_RELEASE
│   ├── Gameplay/                  # Game logic
│   │   └── Game                   # State machine (Attract ↔ Game), rendering, time controls
│   └── EngineBuildPreferences.hpp # Engine feature toggles (V8 scripting disabled)
├── Run/                           # Runtime directory
│   ├── Data/Audio/                # Sound effects (TestSound.mp3)
│   ├── Data/Fonts/                # Bitmap fonts
│   └── Data/Shaders/              # HLSL shaders (Default)
├── Docs/                          # Documentation
└── Protogame2D.sln                # Visual Studio solution
```

## License

Copyright 2025 Yu-Wei Tseng

Licensed under the [Apache License, Version 2.0](../LICENSE).

## Contact

**Yu-Wei Tseng**
- Portfolio: [dadavidtseng.info](https://dadavidtseng.info)
- GitHub: [@dadavidtseng](https://github.com/dadavidtseng)
- LinkedIn: [dadavidtseng](https://www.linkedin.com/in/dadavidtseng/)
- Email: dadavidtseng@gmail.com

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- REFERENCE-STYLE LINKS -->
[license-shield]: https://img.shields.io/github/license/dadavidtseng/Protogame2D.svg?style=for-the-badge
[license-url]: https://github.com/dadavidtseng/Protogame2D/blob/main/LICENSE
