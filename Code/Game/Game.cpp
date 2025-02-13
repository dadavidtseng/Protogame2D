//----------------------------------------------------------------------------------------------------
// Game.cpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

//----------------------------------------------------------------------------------------------------
Game::Game()
{
    m_screenCamera = new Camera();

    Vec2 const bottomLeft     = Vec2::ZERO;
    Vec2 const screenTopRight = Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y);

    m_screenCamera->SetOrthoView(bottomLeft, screenTopRight);
}

//----------------------------------------------------------------------------------------------------
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds)

    // #TODO: Select keyboard or controller
    UpdateFromKeyBoard();
    UpdateFromController();
    AdjustForPauseAndTimeDistortion(deltaSeconds);

    if (g_theInput->WasKeyJustPressed(KEYCODE_TILDE))
    {
        g_theDevConsole->ToggleMode(OPEN_FULL);
    }

    // if (g_theInput->WasKeyJustPressed(KEYCODE_ENTER))
    // {
    //     g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "SHO  OT");
    //     g_theDevConsole->Execute("help a=b");
    // }
}

//----------------------------------------------------------------------------------------------------
void Game::Render() const
{
    g_theRenderer->BeginCamera(*m_screenCamera);

    m_isAttractMode ? RenderAttractMode() : RenderUI();

    // TestDevConsole();

    g_theRenderer->EndCamera(*m_screenCamera);
}

void Game::TestDevConsole() const
{
    AABB2 const box = AABB2(Vec2::ZERO, Vec2(1600.f, 30.f));

    g_theDevConsole->Render(box);
}

//----------------------------------------------------------------------------------------------------
void Game::UpdateFromKeyBoard()
{
    if (g_theInput->WasKeyJustPressed(KEYCODE_SPACE))
    {
        m_isAttractMode          = false;
        SoundID const clickSound = g_theAudio->CreateOrGetSound("Data/Audio/TestSound.mp3");
        g_theAudio->StartSound(clickSound, false, 1.f, 0.f, 0.5f);
    }

    if (g_theInput->WasKeyJustPressed(KEYCODE_ESC))
    {
        m_isAttractMode          = true;
        SoundID const clickSound = g_theAudio->CreateOrGetSound("Data/Audio/TestSound.mp3");
        g_theAudio->StartSound(clickSound);
    }
}

//----------------------------------------------------------------------------------------------------
void Game::UpdateFromController()
{
    XboxController const& controller = g_theInput->GetController(0);

    UNUSED(controller)
}

//----------------------------------------------------------------------------------------------------
void Game::AdjustForPauseAndTimeDistortion(float& deltaSeconds)
{
    UNUSED(deltaSeconds)
}

//----------------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
    DebugDrawRing(Vec2(800.f, 400.f), 300.f, 10.f, Rgba8(255, 127, 0));
}

//----------------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
    DebugDrawLine(Vec2(100.f, 100.f), Vec2(1500.f, 700.f), 10.f, Rgba8(100, 200, 100));
    DebugDrawLine(Vec2(1500.f, 100.f), Vec2(100.f, 700.f), 10.f, Rgba8(100, 200, 100));
}
