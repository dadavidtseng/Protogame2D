//----------------------------------------------------------------------------------------------------
// Game.hpp
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#pragma once

//----------------------------------------------------------------------------------------------------
class Camera;

//----------------------------------------------------------------------------------------------------
class Game
{
public:
    Game();
    ~Game() = default;

    void Update(float deltaSeconds);
    void Render() const;
    bool IsAttractMode() const { return m_isAttractMode; }
    void TestDevConsole() const;

private:
    void UpdateFromKeyBoard();
    void UpdateFromController();
    void AdjustForPauseAndTimeDistortion(float& deltaSeconds);
    void RenderAttractMode() const;
    void RenderUI() const;

    Camera* m_screenCamera  = nullptr;
    bool    m_isAttractMode = true;
};
