//----------------------------------------------------------------------------------------------------
// GameCommon.hpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#pragma once

class ResourceSubsystem;
//-Forward-Declaration--------------------------------------------------------------------------------
struct Rgba8;
struct Vec2;
class App;
class AudioSystem;
class BitmapFont;
class Game;
class Renderer;
class RandomNumberGenerator;

// one-time declaration
extern App*                   g_app;
extern AudioSystem*           g_audio;
extern BitmapFont*            g_bitmapFont;
extern Game*                  g_game;
extern Renderer*              g_renderer;
extern RandomNumberGenerator* g_rng;
extern ResourceSubsystem*     g_resourceSubsystem;

//----------------------------------------------------------------------------------------------------
// DebugRender-related
//
void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);

//----------------------------------------------------------------------------------------------------
template <typename T>
void GAME_SAFE_RELEASE(T*& pointer)
{
    if (pointer != nullptr)
    {
        delete pointer;
        pointer = nullptr;
    }
}
