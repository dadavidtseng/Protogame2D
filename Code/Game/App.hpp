//----------------------------------------------------------------------------------------------------
// App.hpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Timer.hpp"

//-Forward-Declaration--------------------------------------------------------------------------------
class Camera;
class Game;

//----------------------------------------------------------------------------------------------------
class App
{
public:
    App()  = default;
    ~App() = default;
    void Startup();
    void Shutdown();
    void RunFrame();

    void RunMainLoop();

    static bool OnWindowClose(EventArgs& args);
    static bool Event_KeyPressed(EventArgs& args);
    static void RequestQuit();
    static bool m_isQuitting;

private:
    void BeginFrame() const;
    void Update(float deltaSeconds);
    void Render() const;
    void EndFrame() const;

    void HandleKeyPressed();
    void HandleKeyReleased();

    void DeleteAndCreateNewGame();

    float m_timeLastFrameStart = 0.f;

    Camera* m_devConsoleCamera = nullptr;
    Timer* m_timer =nullptr;
};
