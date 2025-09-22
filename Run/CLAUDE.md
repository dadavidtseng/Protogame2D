[Root Directory](../../CLAUDE.md) > **Run**

# Runtime Module

**Changelog**
- *2025-09-22 13:29:24* - Initial documentation: Catalogued runtime assets, executables, and third-party dependencies for Protogame2D deployment

## Module Responsibilities

The Run module contains all runtime assets, compiled executables, and third-party libraries required for Protogame2D execution. This module serves as the deployment directory where the final application and all its dependencies are staged for end-user execution.

### Core Responsibilities
- **Executable Hosting**: Houses compiled game binaries for different configurations
- **Asset Management**: Contains game assets including shaders, fonts, and audio files
- **Dependency Distribution**: Includes all required third-party DLLs and data files
- **Runtime Environment**: Provides complete execution environment for the game

## Entry and Startup

### Executable Files
- **Debug Builds**: 
  - `Protogame2D_Debug_x64.exe` - 64-bit debug configuration
  - `Protogame2D_Debug_x86.exe` - 32-bit debug configuration
- **Release Builds**:
  - `Protogame2D_Release_x64.exe` - 64-bit release configuration  
  - `Protogame2D_Release_x86.exe` - 32-bit release configuration

### Startup Dependencies
All executables require the co-located DLL files for proper initialization and runtime operation.

## External Interfaces

### Asset Access Patterns
The game accesses assets using relative paths from the executable directory:
- **Shaders**: `Data/Shaders/Default.hlsl` (HLSL compilation)
- **Fonts**: `Data/Fonts/DaemonFont` (bitmap font loading)
- **Audio**: `Data/Audio/TestSound.mp3` (sound effect playback)

### File System Structure
```
Run/
├── Protogame2D_[Config]_[Platform].exe    # Game executables
├── *.dll                                  # Third-party dependencies
├── *.dat                                  # Runtime data files
└── Data/                                  # Game assets
    ├── Shaders/                           # HLSL shader files
    ├── Fonts/                             # Bitmap font assets
    └── Audio/                             # Sound effect files
```

## Key Dependencies and Configuration

### Audio Dependencies (FMOD)
- **fmod.dll** - 32-bit FMOD audio library
- **fmod64.dll** - 64-bit FMOD audio library
- **Purpose**: Real-time audio playback and 3D audio processing

### V8 JavaScript Engine Dependencies
- **v8.dll** - V8 JavaScript engine core
- **v8_libbase.dll** - V8 base library
- **v8_libplatform.dll** - V8 platform abstraction
- **Purpose**: Potentially used for scripting or embedded JavaScript functionality

### ICU (International Components for Unicode)
- **icuuc.dll** - ICU Unicode common library
- **icudtl.dat** - ICU data file for Unicode support
- **third_party_icu_icui18n.dll** - ICU internationalization
- **Purpose**: Unicode text processing and internationalization

### Third-Party Libraries
- **third_party_zlib.dll** - Compression library
- **third_party_abseil-cpp_absl.dll** - Abseil C++ common libraries
- **Purpose**: Data compression and utility functions

## Data Models

### Shader Assets
- **Location**: `Data/Shaders/Default.hlsl`
- **Type**: DirectX 11 HLSL shader
- **Structure**: Vertex and Pixel shader stages with PCU vertex format
- **Constants**: Camera matrices, model transforms, texture sampling

### Font Assets
- **DaemonFont.png**: Primary UI font for debug console and text rendering
- **SquirrelFixedFont.png**: Secondary fixed-width font asset
- **Format**: Bitmap font textures with character mapping

### Audio Assets
- **TestSound.mp3**: Sample audio file for game state transitions
- **Format**: MP3 compressed audio
- **Usage**: Triggered during ATTRACT ↔ GAME state changes

### Test Data
- **test.txt**: Simple text file containing test data "AAAAAAAAAA..." and "bbbbbb"
- **Purpose**: Likely for file I/O testing or debugging

## Testing and Quality

### Deployment Testing
1. **Dependency Verification**: Ensure all required DLLs are present
2. **Asset Loading**: Verify shader compilation and asset file access
3. **Cross-Platform**: Test both x86 and x64 configurations
4. **Audio Functionality**: Confirm FMOD library integration

### Asset Integrity
- **Shader Compilation**: HLSL files must be accessible for runtime compilation
- **Font Loading**: Bitmap fonts must be loadable by engine font system
- **Audio Playback**: MP3 files must be compatible with FMOD decoder

### Performance Considerations
- **File Access**: Assets loaded on-demand during gameplay
- **Memory Usage**: Large assets (fonts, audio) affect memory footprint
- **Startup Time**: DLL loading and initialization affects application startup

## FAQ

### Q: Why are there multiple executable variants?
A: Different combinations of Debug/Release builds and x86/x64 architectures provide options for development (Debug) vs. distribution (Release) and platform compatibility.

### Q: What is the V8 engine used for?
A: The V8 JavaScript engine dependencies suggest potential scripting capabilities, though the current game code doesn't show explicit JavaScript usage. This may be a future feature or engine capability.

### Q: How are shaders handled at runtime?
A: The Default.hlsl file indicates runtime shader compilation rather than pre-compiled shaders, allowing for dynamic shader loading and modification.

### Q: Can assets be modified without rebuilding?
A: Yes, assets in the Data/ directory can be modified directly since they're loaded at runtime. Shader changes require application restart for recompilation.

## Related File List

### Executables
- `Protogame2D_Debug_x64.exe` - 64-bit debug build
- `Protogame2D_Release_x64.exe` - 64-bit release build  
- `Protogame2D_Debug_x86.exe` - 32-bit debug build
- `Protogame2D_Release_x86.exe` - 32-bit release build

### Runtime Libraries
- Audio: `fmod.dll`, `fmod64.dll`
- JavaScript: `v8.dll`, `v8_libbase.dll`, `v8_libplatform.dll`
- Unicode: `icuuc.dll`, `icudtl.dat`, `third_party_icu_icui18n.dll`
- Utilities: `third_party_zlib.dll`, `third_party_abseil-cpp_absl.dll`

### Game Assets
- Shaders: `Data/Shaders/Default.hlsl`
- Fonts: `Data/Fonts/DaemonFont.png`, `Data/Fonts/SquirrelFixedFont.png`
- Audio: `Data/Audio/TestSound.mp3`
- Test Data: `Data/test.txt`

## Changelog

- **2025-09-22 13:29:24**: Initial Runtime module documentation
  - Catalogued all executable variants and configurations
  - Identified third-party dependencies and their purposes
  - Documented asset structure and access patterns
  - Analyzed deployment requirements and testing considerations