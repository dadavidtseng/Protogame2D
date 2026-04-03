//----------------------------------------------------------------------------------------------------
// App.cpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#include "Game/Framework/App.hpp"
//----------------------------------------------------------------------------------------------------
#include "Game/Framework/GameCommon.hpp"
#include "Game/Gameplay/Game.hpp"
//----------------------------------------------------------------------------------------------------
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/HashedCaseInsensitiveString.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Resource/ResourceSubsystem.hpp"

//----------------------------------------------------------------------------------------------------
#if defined ERROR
#undef ERROR
#endif

//----------------------------------------------------------------------------------------------------
App*  g_app  = nullptr;     // Created and owned by Main_Windows.cpp
Game* g_game = nullptr;     // Created and owned by the App

//----------------------------------------------------------------------------------------------------
STATIC bool App::m_isQuitting = false;

//----------------------------------------------------------------------------------------------------
App::App()
{
    GEngine::Get().Construct();
}

//----------------------------------------------------------------------------------------------------
App::~App()
{
    GEngine::Get().Destruct();
}

//----------------------------------------------------------------------------------------------------
/// @brief
/// Create all engine subsystems in a specific order.
void App::Startup()
{
    GEngine::Get().Startup();

    g_eventSystem->SubscribeEventCallbackFunction("OnCloseButtonClicked", OnCloseButtonClicked);
    g_eventSystem->SubscribeEventCallbackFunction("quit", OnCloseButtonClicked);

    // SD4-A3a: Register demo console commands
    g_eventSystem->SubscribeEventCallbackFunction("TestHCIS", Command_TestHCIS);
    g_eventSystem->SubscribeEventCallbackFunction("TestProperties", Command_TestProperties);
    g_eventSystem->SubscribeEventCallbackFunction("TestQuotes", Command_TestQuotes);
    g_eventSystem->SubscribeEventCallbackFunction("ChangeMap", Command_ChangeMap);
    g_eventSystem->SubscribeEventCallbackFunction("SpawnActor", Command_SpawnActor);
    g_eventSystem->SubscribeEventCallbackFunction("RunScript", Command_RunScript);
    g_eventSystem->SubscribeEventCallbackFunction("TestTrigger", Command_TestTrigger);

    // SD4-A3a: Subscribe member function event handler
    g_eventSystem->SubscribeEventCallbackObjectMethod("TestMemberEvent", this, &App::Event_TestMember);

    g_game = new Game();
}

//----------------------------------------------------------------------------------------------------
// All Destroy and ShutDown process should be reverse order of the StartUp
//
void App::Shutdown()
{
    GAME_SAFE_RELEASE(g_game);

    g_eventSystem->UnsubscribeEventCallbackFunction("quit", OnCloseButtonClicked);
    g_eventSystem->UnsubscribeEventCallbackFunction("OnCloseButtonClicked", OnCloseButtonClicked);

    GEngine::Get().Shutdown();
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
    bool const        isAttractState       = g_game->IsAttractState();
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

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: Member function event handler
//----------------------------------------------------------------------------------------------------
bool App::Event_TestMember(EventArgs& args)
{
    ++m_testMemberCallCount;

    std::string health = args.GetValue("health", "unknown");

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("[MemberEvent] Call #%d | health=%s", m_testMemberCallCount, health.c_str()));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, "  This was called via SubscribeEventCallbackObjectMethod on App instance");

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: HashedCaseInsensitiveString
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_TestHCIS(EventArgs& args)
{
    UNUSED(args)

    HashedCaseInsensitiveString a("Hello");
    HashedCaseInsensitiveString b("hello");
    HashedCaseInsensitiveString c("HELLO");
    HashedCaseInsensitiveString d("World");

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, "--- HCIS Test ---");
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  a = \"%s\" (hash=%u)", a.c_str(), a.GetHash()));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  b = \"%s\" (hash=%u)", b.c_str(), b.GetHash()));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  c = \"%s\" (hash=%u)", c.c_str(), c.GetHash()));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  d = \"%s\" (hash=%u)", d.c_str(), d.GetHash()));

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  a == b (Hello == hello): %s", (a == b) ? "TRUE" : "FALSE"));
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  a == c (Hello == HELLO): %s", (a == c) ? "TRUE" : "FALSE"));
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  a == d (Hello == World): %s", (a == d) ? "TRUE" : "FALSE"));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  a.GetOriginalString() = \"%s\" (case preserved)", a.GetOriginalString().c_str()));

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: NamedProperties
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_TestProperties(EventArgs& args)
{
    UNUSED(args)

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, "--- NamedProperties Test ---");

    NamedProperties props;

    // Mixed types
    props.SetValue("Health", 100.f);
    props.SetValue("Name", "Hero");
    props.SetValue("Position", Vec2(3.f, 5.f));
    props.SetValue("Color", Rgba8(255, 128, 0, 255));
    props.SetValue("Level", 42);
    props.SetValue("IsAlive", true);

    float       health   = props.GetValue("Health", 0.f);
    std::string name     = props.GetValue("Name", "default");
    Vec2        position = props.GetValue("Position", Vec2::ZERO);
    Rgba8       color    = props.GetValue("Color", Rgba8::WHITE);
    int         level    = props.GetValue("Level", 0);
    bool        isAlive  = props.GetValue("IsAlive", false);

    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  Health (float): %.1f", health));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  Name (string): %s", name.c_str()));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  Position (Vec2): %.1f, %.1f", position.x, position.y));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  Color (Rgba8): %d,%d,%d,%d", color.r, color.g, color.b, color.a));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  Level (int): %d", level));
    g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  IsAlive (bool): %s", isAlive ? "true" : "false"));

    // Default value for missing key
    float missing = props.GetValue("MissingKey", -1.f);
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  MissingKey default: %.1f", missing));

    // Case-insensitive key lookup
    float healthLower = props.GetValue("health", 0.f);
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  Case-insensitive: GetValue(\"health\") = %.1f", healthLower));

    // Backward compat: set as string, get as typed
    props.SetValue("GPA", "3.14");
    float gpa = props.GetValue("GPA", 0.f);
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  Backward compat: SetValue(\"GPA\", \"3.14\") -> GetValue<float> = %.2f", gpa));

    // Nested NamedProperties
    NamedProperties inner;
    inner.SetValue("InnerKey", "InnerValue");
    props.SetValue("Nested", inner);
    NamedProperties retrieved = props.GetValue("Nested", NamedProperties());
    std::string innerVal = retrieved.GetValue("InnerKey", "not found");
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("  Nested: props[\"Nested\"][\"InnerKey\"] = \"%s\"", innerVal.c_str()));

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: Quote-aware parsing
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_TestQuotes(EventArgs& args)
{
    g_devConsole->AddLine(DevConsole::INFO_MAJOR, "--- TestQuotes: Received key-value pairs ---");

    // Iterate all string key-value pairs to show what was parsed
    // Since EventArgs is NamedProperties, we can get string values for known keys
    // For demo, we'll try common test keys
    char const* testKeys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "map", "startPos", "name", "class", "type", "pos", "faction", "file"};

    for (char const* key : testKeys)
    {
        std::string value = args.GetValue(key, "");

        if (!value.empty())
        {
            g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("  %s = \"%s\"", key, value.c_str()));
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: ChangeMap (for XML script demo)
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_ChangeMap(EventArgs& args)
{
    std::string map      = args.GetValue("map", "unknown");
    std::string startPos = args.GetValue("startPos", "0,0");

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("[ChangeMap] map=\"%s\" startPos=\"%s\"", map.c_str(), startPos.c_str()));

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: SpawnActor (for XML script demo)
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_SpawnActor(EventArgs& args)
{
    std::string type    = args.GetValue("type", "unknown");
    std::string pos     = args.GetValue("pos", "0,0");
    std::string name    = args.GetValue("name", "");
    std::string faction = args.GetValue("faction", "neutral");

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("[SpawnActor] type=\"%s\" pos=\"%s\" name=\"%s\" faction=\"%s\"",
        type.c_str(), pos.c_str(), name.c_str(), faction.c_str()));

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: RunScript (XML command script file)
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_RunScript(EventArgs& args)
{
    std::string file = args.GetValue("file", "");

    if (file.empty())
    {
        g_devConsole->AddLine(DevConsole::ERROR, "RunScript: missing 'file' argument. Usage: RunScript file=\"path/to/script.xml\"");
        return false;
    }

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("[RunScript] Executing: %s", file.c_str()));
    g_devConsole->ExecuteXmlCommandScriptFile(file);

    return false;
}

//----------------------------------------------------------------------------------------------------
// SD4-A3a Demo: TestTrigger (embedded XML command script node)
//----------------------------------------------------------------------------------------------------
STATIC bool App::Command_TestTrigger(EventArgs& args)
{
    UNUSED(args)

    g_devConsole->AddLine(DevConsole::INFO_MAJOR, "[TestTrigger] Executing embedded XML trigger block...");

    // Simulate an embedded trigger block like <Trigger type="died">
    char const* triggerXml =
        "<Trigger type=\"died\">"
        "    <ChangeMap map=\"VictoryScreen\" startPos=\"5,5\"/>"
        "    <SpawnActor type=\"TreasureChest\" pos=\"10,10\" name=\"Reward\" faction=\"neutral\"/>"
        "</Trigger>";

    XmlDocument doc;
    doc.Parse(triggerXml);

    XmlElement const* triggerElement = doc.RootElement();

    if (triggerElement)
    {
        g_devConsole->ExecuteXmlCommandScriptNode(*triggerElement);
    }

    return false;
}
