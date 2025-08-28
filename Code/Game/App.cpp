//----------------------------------------------------------------------------------------------------
// App.cpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#include "Game/App.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

//----------------------------------------------------------------------------------------------------
App*                   g_app        = nullptr;       // Created and owned by Main_Windows.cpp
AudioSystem*           g_audio      = nullptr;       // Created and owned by the App
BitmapFont*            g_bitmapFont = nullptr;       // Created and owned by the App
Game*                  g_game       = nullptr;       // Created and owned by the App
Renderer*              g_renderer   = nullptr;       // Created and owned by the App
RandomNumberGenerator* g_rng        = nullptr;       // Created and owned by the App
Window*                g_window     = nullptr;       // Created and owned by the App

//----------------------------------------------------------------------------------------------------
STATIC bool App::m_isQuitting = false;

//----------------------------------------------------------------------------------------------------
App::App()
{
    //-Start-of-EventSystem---------------------------------------------------------------------------

    sEventSystemConfig constexpr sEventSystemConfig;
    g_eventSystem = new EventSystem(sEventSystemConfig);
    g_eventSystem->SubscribeEventCallbackFunction("OnCloseButtonClicked", OnCloseButtonClicked);
    g_eventSystem->SubscribeEventCallbackFunction("quit", OnCloseButtonClicked);

    //-End-of-EventSystem-----------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-InputSystem---------------------------------------------------------------------------

    sInputSystemConfig constexpr sInputSystemConfig;
    g_input = new InputSystem(sInputSystemConfig);

    //-End-of-InputSystem-----------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-Window--------------------------------------------------------------------------------

    sWindowConfig windowConfig;
    windowConfig.m_windowType  = eWindowType::WINDOWED;
    windowConfig.m_aspectRatio = 2.f;
    windowConfig.m_inputSystem = g_input;
    windowConfig.m_windowTitle = "Protogame3D";
    g_window                   = new Window(windowConfig);

    //-End-of-Window----------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-Renderer------------------------------------------------------------------------------

    sRendererConfig sRendererConfig;
    sRendererConfig.m_window = g_window;
    g_renderer               = new Renderer(sRendererConfig);

    //-End-of-Renderer--------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-DevConsole----------------------------------------------------------------------------

    sDevConsoleConfig sDevConsoleConfig;
    sDevConsoleConfig.m_defaultRenderer = g_renderer;
    sDevConsoleConfig.m_defaultFontName = "DaemonFont";
    m_devConsoleCamera                  = new Camera();
    sDevConsoleConfig.m_defaultCamera   = m_devConsoleCamera;
    g_devConsole                        = new DevConsole(sDevConsoleConfig);

    //-End-of-DevConsole------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-AudioSystem---------------------------------------------------------------------------

    sAudioSystemConfig constexpr sAudioSystemConfig;
    g_audio = new AudioSystem(sAudioSystemConfig);

    //-End-of-AudioSystem-----------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------
App::~App()
{
    GAME_SAFE_RELEASE(g_game);
    GAME_SAFE_RELEASE(g_rng);
    GAME_SAFE_RELEASE(g_bitmapFont);
    GAME_SAFE_RELEASE(m_devConsoleCamera);
    GAME_SAFE_RELEASE(g_audio);
    GAME_SAFE_RELEASE(g_renderer);
    GAME_SAFE_RELEASE(g_window);
    GAME_SAFE_RELEASE(g_input);
}

//----------------------------------------------------------------------------------------------------
/// @brief
/// Create all engine subsystems in a specific order.
void App::Startup()
{
    //-Start-of-DebugRender---------------------------------------------------------------------------

    sDebugRenderConfig sDebugRenderConfig;
    sDebugRenderConfig.m_renderer = g_renderer;
    sDebugRenderConfig.m_fontName = "DaemonFont";

    //-End-of-DebugRender-----------------------------------------------------------------------------

    g_eventSystem->Startup();
    g_window->Startup();
    g_renderer->Startup();
    DebugRenderSystemStartup(sDebugRenderConfig);
    g_devConsole->StartUp();
    g_input->Startup();
    g_audio->Startup();

    g_bitmapFont = g_renderer->CreateOrGetBitmapFontFromFile("Data/Fonts/DaemonFont"); // DO NOT SPECIFY FILE .EXTENSION!!  (Important later on.)
    g_rng        = new RandomNumberGenerator();
    g_game       = new Game();
}

//----------------------------------------------------------------------------------------------------
// All Destroy and ShutDown process should be reverse order of the StartUp
//
void App::Shutdown()
{
    g_audio->Shutdown();
    g_input->Shutdown();
    g_devConsole->Shutdown();
    DebugRenderSystemShutdown();
    g_renderer->Shutdown();
    g_window->Shutdown();
    g_eventSystem->Shutdown();
}

//----------------------------------------------------------------------------------------------------
void App::RunMainLoop()
{
    // Program main loop; keep running frames until it's time to quit
    while (!m_isQuitting)
    {
        // Sleep(16); // Temporary code to "slow down" our app to ~60Hz until we have proper frame timing in
        RunFrame();
    }
}

//----------------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void App::RunFrame()
{
    BeginFrame();   // Engine pre-frame stuff
    Update();       // Game updates / moves / spawns / hurts / kills stuff
    Render();       // Game draws current state of things
    EndFrame();     // Engine post-frame stuff
}

//----------------------------------------------------------------------------------------------------
STATIC bool App::OnCloseButtonClicked(EventArgs& args)
{
    UNUSED(args)

    RequestQuit();

    return false;
}

//----------------------------------------------------------------------------------------------------
STATIC void App::RequestQuit()
{
    m_isQuitting = true;
}

//----------------------------------------------------------------------------------------------------
void App::BeginFrame() const
{
    g_eventSystem->BeginFrame();
    g_window->BeginFrame();
    g_renderer->BeginFrame();
    DebugRenderBeginFrame();
    g_devConsole->BeginFrame();
    g_input->BeginFrame();
    g_audio->BeginFrame();
}

//----------------------------------------------------------------------------------------------------
void App::Update()
{
    Clock::TickSystemClock();
    UpdateCursorMode();
    g_game->Update();
}

//----------------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
//
// Ultimately this function (App::Render) will only call methods on Renderer (like Renderer::DrawVertexArray)
//	to draw things, never calling OpenGL (nor DirectX) functions directly.
//
void App::Render() const
{
    Rgba8 const clearColor = Rgba8::GREY;

    g_renderer->ClearScreen(clearColor);
    g_game->Render();

    AABB2 const box = AABB2(Vec2::ZERO, Vec2(1600.f, 30.f));

    g_devConsole->Render(box);
}

//----------------------------------------------------------------------------------------------------
void App::EndFrame() const
{
    g_eventSystem->EndFrame();
    g_window->EndFrame();
    g_renderer->EndFrame();
    DebugRenderEndFrame();
    g_devConsole->EndFrame();
    g_input->EndFrame();
    g_audio->EndFrame();
}

//----------------------------------------------------------------------------------------------------
void App::UpdateCursorMode()
{
    bool const        doesWindowHasFocus   = GetActiveWindow() == g_window->GetWindowHandle();
    bool const        isAttractState       = g_game->GetCurrentGameState() == eGameState::ATTRACT;
    bool const        shouldUsePointerMode = !doesWindowHasFocus || g_devConsole->IsOpen() || isAttractState;
    eCursorMode const mode                 = shouldUsePointerMode ? eCursorMode::POINTER : eCursorMode::FPS;

    g_input->SetCursorMode(mode);
}

//----------------------------------------------------------------------------------------------------
void App::DeleteAndCreateNewGame()
{
    GAME_SAFE_RELEASE(g_game);
    g_game = new Game();
}
