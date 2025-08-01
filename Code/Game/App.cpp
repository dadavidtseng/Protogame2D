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
RandomNumberGenerator* g_rng     = nullptr;       // Created and owned by the App
Window*                g_window  = nullptr;       // Created and owned by the App

//----------------------------------------------------------------------------------------------------
STATIC bool App::m_isQuitting = false;

//----------------------------------------------------------------------------------------------------
/// @brief
/// Create all engine subsystems in a specific order.
void App::Startup()
{
    //-Start-of-EventSystem---------------------------------------------------------------------------

    sEventSystemConfig constexpr sEventSystemConfig;
    g_theEventSystem = new EventSystem(sEventSystemConfig);
    g_theEventSystem->SubscribeEventCallbackFunction("OnCloseButtonClicked", OnWindowClose);
    g_theEventSystem->SubscribeEventCallbackFunction("quit", OnWindowClose);

    //-End-of-EventSystem-----------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-InputSystem---------------------------------------------------------------------------

    sInputSystemConfig constexpr sInputSystemConfig;
    g_theInput = new InputSystem(sInputSystemConfig);

    //-End-of-InputSystem-----------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-Window--------------------------------------------------------------------------------

    sWindowConfig sWindowConfig;
    sWindowConfig.m_windowType  = eWindowType::WINDOWED;
    sWindowConfig.m_aspectRatio = 2.f;
    sWindowConfig.m_inputSystem = g_theInput;
    sWindowConfig.m_windowTitle = "Protogame2D";
    g_window                 = new Window(sWindowConfig);

    //-End-of-Window----------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-Renderer------------------------------------------------------------------------------

    sRendererConfig sRendererConfig;
    sRendererConfig.m_window = g_window;
    g_renderer               = new Renderer(sRendererConfig);

    //-End-of-Renderer--------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-DebugRender---------------------------------------------------------------------------

    sDebugRenderConfig sDebugRenderConfig;
    sDebugRenderConfig.m_renderer = g_renderer;
    sDebugRenderConfig.m_fontName = "SquirrelFixedFont";

    //-End-of-DebugRender-----------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-DevConsole----------------------------------------------------------------------------

    m_devConsoleCamera = new Camera();

    Vec2 const bottomLeft     = Vec2::ZERO;
    Vec2 const screenTopRight = Window::s_mainWindow->GetClientDimensions();

    m_devConsoleCamera->SetOrthoGraphicView(bottomLeft, screenTopRight);

    sDevConsoleConfig sDevConsoleConfig;
    sDevConsoleConfig.m_defaultRenderer = g_renderer;
    sDevConsoleConfig.m_defaultFontName = "SquirrelFixedFont";
    sDevConsoleConfig.m_defaultCamera   = m_devConsoleCamera;
    g_theDevConsole                     = new DevConsole(sDevConsoleConfig);

    //-End-of-DevConsole------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    //-Start-of-AudioSystem---------------------------------------------------------------------------

    sAudioSystemConfig constexpr sAudioSystemConfig;
    g_audio = new AudioSystem(sAudioSystemConfig);

    //-End-of-AudioSystem-----------------------------------------------------------------------------

    g_theEventSystem->Startup();
    g_window->Startup();
    g_renderer->Startup();
    DebugRenderSystemStartup(sDebugRenderConfig);
    g_theDevConsole->StartUp();
    g_theInput->Startup();
    g_audio->Startup();

    g_bitmapFont = g_renderer->CreateOrGetBitmapFontFromFile("Data/Fonts/SquirrelFixedFont"); // DO NOT SPECIFY FILE .EXTENSION!!  (Important later on.)
    g_rng     = new RandomNumberGenerator();
    g_game       = new Game();
}

//----------------------------------------------------------------------------------------------------
// All Destroy and ShutDown process should be reverse order of the StartUp
//
void App::Shutdown()
{
    // Destroy all Engine Subsystem
    GAME_SAFE_RELEASE(g_game);
    GAME_SAFE_RELEASE(g_rng);
    GAME_SAFE_RELEASE(g_bitmapFont);

    g_audio->Shutdown();
    g_theInput->Shutdown();
    g_theDevConsole->Shutdown();

    GAME_SAFE_RELEASE(m_devConsoleCamera);

    DebugRenderSystemShutdown();
    g_renderer->Shutdown();
    g_window->Shutdown();
    g_theEventSystem->Shutdown();

    GAME_SAFE_RELEASE(g_audio);
    GAME_SAFE_RELEASE(g_renderer);
    GAME_SAFE_RELEASE(g_window);
    GAME_SAFE_RELEASE(g_theInput);
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
STATIC bool App::OnWindowClose(EventArgs& args)
{
    UNUSED(args)

    RequestQuit();

    return true;
}

//----------------------------------------------------------------------------------------------------
STATIC void App::RequestQuit()
{
    m_isQuitting = true;
}

//----------------------------------------------------------------------------------------------------
void App::BeginFrame() const
{
    g_theEventSystem->BeginFrame();
    g_window->BeginFrame();
    g_renderer->BeginFrame();
    DebugRenderBeginFrame();
    g_theDevConsole->BeginFrame();
    g_theInput->BeginFrame();
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

    g_theDevConsole->Render(box);
}

//----------------------------------------------------------------------------------------------------
void App::EndFrame() const
{
    g_theEventSystem->EndFrame();
    g_window->EndFrame();
    g_renderer->EndFrame();
    DebugRenderEndFrame();
    g_theDevConsole->EndFrame();
    g_theInput->EndFrame();
    g_audio->EndFrame();
}

//----------------------------------------------------------------------------------------------------
void App::UpdateCursorMode() const
{
    bool const        doesWindowHasFocus   = GetActiveWindow() == g_window->GetWindowHandle();
    bool const        isAttractState       = g_game->GetCurrentGameState() == eGameState::ATTRACT;
    bool const        shouldUsePointerMode = !doesWindowHasFocus || g_theDevConsole->IsOpen() || isAttractState;
    eCursorMode const mode                 = shouldUsePointerMode ? eCursorMode::POINTER : eCursorMode::FPS;

    g_theInput->SetCursorMode(mode);
}
