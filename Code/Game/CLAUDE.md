[Root Directory](../../CLAUDE.md) > [Code](../) > **Game**

# Game Module

**Changelog**
- *2025-09-22 13:29:24* - Initial documentation: Analyzed core game loop architecture, application lifecycle, and DirectX 11 integration patterns

## Module Responsibilities

The Game module serves as the primary application layer, managing the complete lifecycle of the Protogame2D application. It orchestrates engine subsystems, handles user input, manages game states, and provides the main entry point for the Windows application.

### Core Responsibilities
- **Application Lifecycle**: Startup, main loop execution, and shutdown coordination
- **Subsystem Management**: Initialization and coordination of audio, rendering, input, and window systems
- **Game State Management**: Transitions between ATTRACT and GAME states with event-driven architecture
- **Input Handling**: Keyboard input processing for game controls and system commands
- **Rendering Coordination**: Managing cameras, render calls, and debug visualization

## Entry and Startup

### Main Entry Point
- **File**: `Main_Windows.cpp`
- **Function**: `WinMain()` - Standard Windows application entry point
- **Flow**: Creates App instance → Startup() → RunMainLoop() → Shutdown() → Cleanup

### Initialization Sequence
1. **EventSystem**: Event handling and callback registration
2. **InputSystem**: Keyboard and mouse input management  
3. **Window**: Win32 window creation with DirectX context
4. **Renderer**: DirectX 11 renderer initialization
5. **DevConsole**: Debug console with font rendering
6. **AudioSystem**: FMOD audio system startup
7. **Game Logic**: Game instance creation and camera setup

## External Interfaces

### App Class Public Interface
```cpp
class App {
public:
    App();                              // System initialization
    ~App();                             // Resource cleanup
    void Startup();                     // Engine subsystem startup
    void Shutdown();                    // Engine subsystem shutdown
    void RunMainLoop();                 // Main application loop
    void RunFrame();                    // Single frame execution
    static void RequestQuit();          // Graceful application termination
    static bool OnCloseButtonClicked(EventArgs& args);  // Window close event handler
};
```

### Game Class Public Interface
```cpp
class Game {
public:
    Game();                             // Game initialization
    ~Game();                            // Game cleanup
    void Update();                      // Per-frame game logic
    void Render() const;                // Per-frame rendering
    eGameState GetCurrentGameState() const;           // State query
    void ChangeGameState(eGameState newGameState);    // State transitions
};
```

### Game States
- **ATTRACT**: Title screen mode with disc animation and fullscreen toggle
- **GAME**: Interactive mode with crossed lines visualization and debug info

## Key Dependencies and Configuration

### External Engine Dependencies
- **Path**: `../Engine/Code/` (relative to solution)
- **Project Reference**: `Engine.vcxproj` 
- **Key Systems**: Core, Audio, Input, Platform, Renderer, Math modules

### Third-Party Libraries
- **DirectX 11**: Graphics API and HLSL shader compilation
- **FMOD**: Audio playback and 3D audio processing
- **Windows SDK**: Platform-specific window and input handling

### Build Configuration
- **Project File**: `Game.vcxproj`
- **Language Standard**: C++20 for Debug, C++17 for Release
- **Include Paths**: `$(SolutionDir)Code/` and `$(SolutionDir)../Engine/Code/`
- **Output**: Executables copied to `$(SolutionDir)Run/` via post-build event

### Engine Build Preferences
- **File**: `EngineBuildPreferences.hpp`
- **Audio**: FMOD enabled (comment `ENGINE_DISABLE_AUDIO` to disable)
- **Debug Rendering**: Enabled via `ENGINE_DEBUG_RENDER` define

## Data Models

### Global System References
```cpp
extern App*                   g_app;        // Application instance
extern AudioSystem*           g_audio;      // Audio subsystem
extern BitmapFont*            g_bitmapFont; // Font rendering
extern Game*                  g_game;       // Game logic instance
extern Renderer*              g_renderer;   // Graphics renderer
extern RandomNumberGenerator* g_rng;        // Random number generation
```

### Game State Model
```cpp
enum class eGameState : int8_t {
    ATTRACT,    // Title/menu screen
    GAME        // Active gameplay
};
```

### Frame Structure
- **BeginFrame()**: System preparation for frame
- **Update()**: Game logic processing with time management
- **Render()**: Graphics rendering with camera management
- **EndFrame()**: System cleanup and presentation

## Testing and Quality

### Current Status
- **Unit Tests**: None implemented
- **Integration Tests**: Manual testing via executable
- **Code Quality**: High-level architecture with clear separation of concerns

### Manual Testing Procedures
1. **State Transitions**: Verify SPACE key transitions between attract/game modes
2. **Input Handling**: Test ESC for quit/return, R for fullscreen, P/O/T for time controls
3. **Audio**: Confirm sound playback during state transitions
4. **Rendering**: Visual verification of disc (attract) and crossed lines (game) rendering

### Quality Measures
- **Memory Management**: Custom `GAME_SAFE_RELEASE` macro for consistent cleanup
- **Error Handling**: Event-driven architecture for graceful state management
- **Performance**: Debug timing display showing FPS and time scale information

## FAQ

### Q: How do I add new game states?
A: Extend the `eGameState` enum, add handling in `Game::ChangeGameState()`, implement rendering in `Game::Render()`, and add input handling in `Game::UpdateFromInput()`.

### Q: How do I integrate new engine subsystems?
A: Add initialization in `App::App()` constructor, startup in `App::Startup()`, update calls in frame cycle, and cleanup in `App::~App()` destructor.

### Q: Why are there separate Debug and Release C++ standards?
A: Debug uses C++20 for modern development features, Release uses C++17 for broader compatibility. This is configured in the .vcxproj file.

### Q: How does the engine dependency work?
A: The Engine project is located at `../Engine/` relative to this solution. It provides core systems that this game module builds upon, following a typical engine-game separation pattern.

## Related File List

### Core Implementation
- `Main_Windows.cpp` - Application entry point and Windows integration
- `App.hpp/cpp` - Application lifecycle and subsystem management
- `Game.hpp/cpp` - Game state management and rendering logic
- `GameCommon.hpp` - Global variables and utility functions

### Configuration and Project
- `Game.vcxproj` - Visual Studio project configuration
- `EngineBuildPreferences.hpp` - Engine compilation directives

### Utility and Common
- `GameCommon.cpp` - Implementation of shared functionality

## Changelog

- **2025-09-22 13:29:24**: Initial Game module documentation
  - Documented application architecture and initialization sequence
  - Analyzed game state management and rendering pipeline
  - Identified engine dependencies and build configuration
  - Mapped input handling and frame structure patterns