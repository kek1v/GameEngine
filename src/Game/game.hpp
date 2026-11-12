#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include "World.hpp"
#include <vec2.hpp>

class Tank;
namespace RendererEngine { class Sprite; }

class game {
public:
    explicit game(const glm::ivec2& windowSize);
    ~game();
    bool init();
    void shutdown();
    void render();
    void update(uint64_t delta);
    void setKey(int key, int action);
private:
    std::array<bool, 349> m_Keys{};
    World m_world;
    std::unique_ptr<Tank> m_pTank;
    std::shared_ptr<RendererEngine::Sprite> m_floor, m_wall, m_crate, m_shell;
    std::shared_ptr<RendererEngine::Sprite> m_enemySprite, m_hud, m_result;
    int m_lastHealth = -1, m_lastEnemies = -1;
    World::State m_lastState = World::State::Playing;
};

