//----------------------------------------------------------------------------------------------------
// Game.cpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

//----------------------------------------------------------------------------------------------------
Game::Game()
{
    g_eventSystem->SubscribeEventCallbackFunction("OnGameStateChanged", OnGameStateChanged);

    m_screenCamera = new Camera();

    Vec2 const bottomLeft     = Vec2::ZERO;
    Vec2 const screenTopRight = Window::s_mainWindow->GetClientDimensions();

    m_screenCamera->SetOrthoGraphicView(bottomLeft, screenTopRight);
    m_screenCamera->SetNormalizedViewport(AABB2::ZERO_TO_ONE);

    m_gameClock = new Clock(Clock::GetSystemClock());
}

//----------------------------------------------------------------------------------------------------
Game::~Game()
{
    GAME_SAFE_RELEASE(m_screenCamera);
}

//----------------------------------------------------------------------------------------------------
void Game::Update()
{
    DebugAddScreenText(Stringf("Time: %.2f\nFPS: %.2f\nScale: %.1f", m_gameClock->GetTotalSeconds(), 1.f / m_gameClock->GetDeltaSeconds(), m_gameClock->GetTimeScale()), m_screenCamera->GetOrthographicTopRight() - Vec2(250.f, 60.f), 20.f, Vec2::ZERO, 0.f, Rgba8::WHITE, Rgba8::WHITE);
    UpdateFromInput();
    AdjustForPauseAndTimeDistortion();
}

//----------------------------------------------------------------------------------------------------
void Game::Render() const
{
    //-Start-of-Screen-Camera-------------------------------------------------------------------------
    g_renderer->BeginCamera(*m_screenCamera);

    if (m_gameState == eGameState::ATTRACT)
    {
        RenderAttractMode();
    }
    else if (m_gameState == eGameState::GAME)
    {
        RenderGame();
    }

    g_renderer->EndCamera(*m_screenCamera);
    //-End-of-Screen-Camera---------------------------------------------------------------------------

    if (m_gameState == eGameState::GAME)
    {
        DebugRenderScreen(*m_screenCamera);
    }
}

//----------------------------------------------------------------------------------------------------
bool Game::OnGameStateChanged(EventArgs& args)
{
    String const newGameState = args.GetValue("OnGameStateChanged", "DEFAULT");

    if (newGameState == "ATTRACT")
    {
        SoundID const clickSound = g_audio->CreateOrGetSound("Data/Audio/TestSound.mp3", eAudioSystemSoundDimension::Sound2D);
        g_audio->StartSound(clickSound);
    }
    else if (newGameState == "GAME")
    {
        SoundID const clickSound = g_audio->CreateOrGetSound("Data/Audio/TestSound.mp3", eAudioSystemSoundDimension::Sound2D);
        g_audio->StartSound(clickSound, false, 1.f, 0.f, 0.5f);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
eGameState Game::GetCurrentGameState() const
{
    return m_gameState;
}

//----------------------------------------------------------------------------------------------------
void Game::ChangeGameState(eGameState const newGameState)
{
    if (newGameState == m_gameState) return;

    EventArgs args;

    if (newGameState == eGameState::ATTRACT) args.SetValue("OnGameStateChanged", "ATTRACT");
    else if (newGameState == eGameState::GAME) args.SetValue("OnGameStateChanged", "GAME");

    m_gameState = newGameState;

    g_eventSystem->FireEvent("OnGameStateChanged", args);
}

//----------------------------------------------------------------------------------------------------
void Game::UpdateFromInput()
{
    if (m_gameState == eGameState::ATTRACT)
    {
        if (g_input->WasKeyJustPressed(KEYCODE_R))
        {
            Window::s_mainWindow->SetWindowType(eWindowType::FULLSCREEN_STRETCH);
            Window::s_mainWindow->ReconfigureWindow();
        }

        if (g_input->WasKeyJustPressed(KEYCODE_ESC))
        {
            App::RequestQuit();
        }

        if (g_input->WasKeyJustPressed(KEYCODE_SPACE))
        {
            ChangeGameState(eGameState::GAME);
        }
    }
    else if (m_gameState == eGameState::GAME)
    {
        if (g_input->WasKeyJustPressed(KEYCODE_ESC))
        {
            ChangeGameState(eGameState::ATTRACT);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void Game::AdjustForPauseAndTimeDistortion() const
{
    if (g_input->WasKeyJustPressed(KEYCODE_P)) m_gameClock->TogglePause();
    else if (g_input->WasKeyJustPressed(KEYCODE_O)) m_gameClock->StepSingleFrame();
    else if (g_input->IsKeyDown(KEYCODE_T)) m_gameClock->SetTimeScale(0.1f);
    else if (g_input->WasKeyJustReleased(KEYCODE_T)) m_gameClock->SetTimeScale(1.f);
}

//----------------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
    Vec2 const clientDimensions = Window::s_mainWindow->GetClientDimensions();

    VertexList_PCU verts;
    AddVertsForDisc2D(verts, Vec2(clientDimensions.x * 0.5f, clientDimensions.y * 0.5f), 300.f, 10.f, Rgba8::YELLOW);
    g_renderer->SetModelConstants();
    g_renderer->SetBlendMode(eBlendMode::OPAQUE);
    g_renderer->SetRasterizerMode(eRasterizerMode::SOLID_CULL_BACK);
    g_renderer->SetSamplerMode(eSamplerMode::BILINEAR_CLAMP);
    g_renderer->SetDepthMode(eDepthMode::DISABLED);
    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(g_renderer->CreateOrGetShaderFromFile("Data/Shaders/Default"));
    g_renderer->DrawVertexArray(verts);
}

//----------------------------------------------------------------------------------------------------
void Game::RenderGame() const
{
    Vec2 const clientDimensions = Window::s_mainWindow->GetClientDimensions();
    Vec2 const offsetFromCorner = Vec2(200.f, 100.f);
    Vec2 const topRight         = clientDimensions - offsetFromCorner;
    Vec2 const bottomLeft       = offsetFromCorner;
    Vec2 const topLeft          = Vec2(offsetFromCorner.x, clientDimensions.y - offsetFromCorner.y);
    Vec2 const bottomRight      = Vec2(clientDimensions.x - offsetFromCorner.x, offsetFromCorner.y);

    VertexList_PCU verts;

    AddVertsForLineSegment2D(verts, topRight, bottomLeft, 10.f, false, Rgba8::GREEN);
    AddVertsForLineSegment2D(verts, topLeft, bottomRight, 10.f, false, Rgba8::GREEN);

    g_renderer->SetModelConstants();
    g_renderer->SetBlendMode(eBlendMode::OPAQUE);
    g_renderer->SetRasterizerMode(eRasterizerMode::SOLID_CULL_BACK);
    g_renderer->SetSamplerMode(eSamplerMode::BILINEAR_CLAMP);
    g_renderer->SetDepthMode(eDepthMode::DISABLED);
    g_renderer->BindTexture(nullptr);
    g_renderer->BindShader(g_renderer->CreateOrGetShaderFromFile("Data/Shaders/Default"));
    g_renderer->DrawVertexArray(verts);
}
