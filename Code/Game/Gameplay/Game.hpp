//----------------------------------------------------------------------------------------------------
// Game.hpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------
#include "Engine/Core/EventSystem.hpp"

//-Forward-Declaration--------------------------------------------------------------------------------
class Camera;
class Clock;

//----------------------------------------------------------------------------------------------------
enum class eGameState : int8_t
{
    ATTRACT,
    GAME
};

//----------------------------------------------------------------------------------------------------
class Game
{
public:
    Game();
    ~Game();

    void Update();
    void Render() const;

    eGameState GetCurrentGameState() const;
    void       ChangeGameState(eGameState newState);

private:
    static bool OnGameStateChanged(EventArgs& args);
    void        UpdateFromInput();
    void        AdjustForPauseAndTimeDistortion() const;
    void        RenderAttractMode() const;
    void        RenderGame() const;

    eGameState m_gameState    = eGameState::ATTRACT;
    Camera*    m_screenCamera = nullptr;
    Clock*     m_gameClock    = nullptr;
};
