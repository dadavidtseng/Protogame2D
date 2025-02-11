//----------------------------------------------------------------------------------------------------
// App.hpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Core/EventSystem.hpp"

//----------------------------------------------------------------------------------------------------
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

    static bool OnWindowClose(EventArgs& arg);
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
};


