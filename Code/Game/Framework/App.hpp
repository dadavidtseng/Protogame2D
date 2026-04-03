//----------------------------------------------------------------------------------------------------
// App.hpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------
#include "Engine/Core/EventRecipient.hpp"
#include "Engine/Core/EventSystem.hpp"

//-Forward-Declaration--------------------------------------------------------------------------------
class Camera;

//----------------------------------------------------------------------------------------------------
class App : public EventRecipient
{
public:
    App();
    ~App();

    void Startup();
    void Shutdown();

    void RunMainLoop();
    void RunFrame();

    static bool OnCloseButtonClicked(EventArgs& args);
    static void RequestQuit();
    static bool m_isQuitting;
    void        DeleteAndCreateNewGame();

    // SD4-A3a: Member function event handler demo
    bool Event_TestMember(EventArgs& args);

    // SD4-A3a: Static demo command handlers
    static bool Command_TestHCIS(EventArgs& args);
    static bool Command_TestProperties(EventArgs& args);
    static bool Command_TestQuotes(EventArgs& args);
    static bool Command_ChangeMap(EventArgs& args);
    static bool Command_SpawnActor(EventArgs& args);
    static bool Command_RunScript(EventArgs& args);
    static bool Command_TestTrigger(EventArgs& args);

private:
    void BeginFrame() const;
    void Update();
    void Render() const;
    void EndFrame() const;

    void UpdateCursorMode();

    int m_testMemberCallCount = 0;
};
