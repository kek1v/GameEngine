#include "game.hpp"
#include "Tank.hpp"
#include "Scoreboard.hpp"
#include "../Resources/ResourceManager.hpp"
#include "../Renderer/ShaderProgram.hpp"
#include "../Renderer/AnimatedSprite.hpp"
#include "../Renderer/Texture2D.hpp"
#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <vector>

namespace {
std::shared_ptr<RendererEngine::Sprite> makeTile(
    const std::shared_ptr<RendererEngine::ShaderProgram>& shader, int kind) {
    constexpr int n = 16;
    std::vector<unsigned char> pixels(n * n * 4);
    for (int y = 0; y < n; ++y) for (int x = 0; x < n; ++x) {
        int r = 36, g = 45, b = 43;
        if (kind == 0) {
            const int noise = ((x * 17 + y * 31) % 7) - 3;
            r += noise; g += noise; b += noise;
            if (x == 0 || y == 0) { r = 28; g = 36; b = 35; }
        } else if (kind == 1) {
            const bool mortar = y % 8 == 0 || (x + (y / 8) * 8) % 16 == 0;
            const bool edge = y % 8 == 1;
            r = mortar ? 38 : edge ? 119 : 87;
            g = mortar ? 44 : edge ? 135 : 102;
            b = mortar ? 44 : edge ? 130 : 99;
        } else if (kind == 2) {
            const bool frame = x < 2 || x > 13 || y < 2 || y > 13;
            const bool brace = x == y || x == y + 1 || x + y == 15 || x + y == 14;
            const bool seam = x % 5 == 0;
            r = frame ? 91 : brace ? 202 : seam ? 119 : 157;
            g = frame ? 57 : brace ? 153 : seam ? 79 : 110;
            b = frame ? 30 : brace ? 77 : seam ? 39 : 54;
        } else {
            r = 255; g = 225; b = 128;
        }
        const int i = (y * n + x) * 4;
        pixels[i] = static_cast<unsigned char>(r);
        pixels[i + 1] = static_cast<unsigned char>(g);
        pixels[i + 2] = static_cast<unsigned char>(b);
        pixels[i + 3] = 255;
    }
    auto texture = std::make_shared<RendererEngine::Texture2D>(
        n, n, pixels.data(), 4, GL_NEAREST, GL_CLAMP_TO_EDGE);
    texture->addSubTexture("tile", {0.f, 0.f}, {1.f, 1.f});
    return std::make_shared<RendererEngine::Sprite>(
        texture, "tile", shader, glm::vec2(0.f), glm::vec2(World::TileSize));
}
}

game::game(const glm::ivec2&) {}
game::~game() = default;

bool game::init() {
    if (!ResourceManager::loadJSONresources("res/resources.json")) return false;
    auto shader = ResourceManager::getShaderProgram("SpriteShader");
    auto tankSprite = ResourceManager::getAnimatedSprite("tankAnimatedSprite");
    if (!shader || !tankSprite) return false;
    shader->use();
    shader->setInt("tex", 0);
    shader->setMatrix4("projectionMat", glm::ortho(
        0.f, float(World::Columns * World::TileSize),
        0.f, float(World::Rows * World::TileSize), -1.f, 1.f));
    m_floor = makeTile(shader, 0);
    m_wall = makeTile(shader, 1);
    m_crate = makeTile(shader, 2);
    m_shell = makeTile(shader, 3);
    m_enemySprite = std::make_shared<RendererEngine::Sprite>(
        ResourceManager::getTexture("TanksTextureAtlas"), "yelowType1_Top1", shader,
        glm::vec2(0.f), glm::vec2(World::TankSize));
    tankSprite->setSize(glm::vec2(World::TankSize));
    tankSprite->setState("tankTopState");
    const auto spawn = m_world.tank();
    m_pTank = std::make_unique<Tank>(tankSprite, 0.f, glm::vec2(spawn.x, spawn.y));
    return true;
}

void game::shutdown() {
    m_pTank.reset();
    m_floor.reset(); m_wall.reset(); m_crate.reset(); m_shell.reset();
    m_enemySprite.reset(); m_hud.reset(); m_result.reset();
}

void game::setKey(int key, int action) {
    if (key < 0 || key >= static_cast<int>(m_Keys.size())) return;
    m_Keys[key] = action != GLFW_RELEASE;
    if (key == GLFW_KEY_R && action == GLFW_PRESS) m_world.reset();
}

void game::update(uint64_t delta) {
    if (!m_pTank) return;
    int x = 0, y = 0;
    if (m_Keys[GLFW_KEY_W] || m_Keys[GLFW_KEY_UP]) y = 1;
    else if (m_Keys[GLFW_KEY_A] || m_Keys[GLFW_KEY_LEFT]) x = -1;
    else if (m_Keys[GLFW_KEY_S] || m_Keys[GLFW_KEY_DOWN]) y = -1;
    else if (m_Keys[GLFW_KEY_D] || m_Keys[GLFW_KEY_RIGHT]) x = 1;
    m_world.update(static_cast<float>(delta * 1e-9), x, y, m_Keys[GLFW_KEY_SPACE]);
    const auto direction = m_world.direction();
    m_pTank->setOrientation(direction.x > 0 ? Tank::eOrientation::right :
        direction.x < 0 ? Tank::eOrientation::left :
        direction.y > 0 ? Tank::eOrientation::Top : Tank::eOrientation::Bottom);
    const auto position = m_world.tank();
    m_pTank->setPosition({position.x, position.y});
    m_pTank->move(m_world.moving());
    m_pTank->update(delta);
}

void game::render() {
    if (!m_pTank) return;
    auto shader = ResourceManager::getShaderProgram("SpriteShader");
    shader->use();
    shader->setInt("enemyTint", 0);
    for (int y = 0; y < World::Rows; ++y) for (int x = 0; x < World::Columns; ++x) {
        const glm::vec2 position(float(x * World::TileSize), float(y * World::TileSize));
        auto tile = m_world.tile(x, y);
        auto sprite = tile == World::Tile::Wall ? m_wall :
                      tile == World::Tile::Crate ? m_crate : m_floor;
        sprite->setPosition(position);
        sprite->render();
    }
    m_pTank->render();
    shader->setInt("enemyTint", 1);
    for (const auto& enemy : m_world.enemies()) {
        if (!enemy.alive) continue;
        m_enemySprite->setPosition({enemy.position.x, enemy.position.y});
        m_enemySprite->setRotation(enemy.direction.x > 0 ? -90.f :
            enemy.direction.x < 0 ? 90.f : enemy.direction.y < 0 ? 180.f : 0.f);
        m_enemySprite->render();
    }
    for (const auto& bullet : m_world.bullets()) {
        shader->setInt("enemyTint", bullet.hostile ? 1 : 0);
        const glm::vec2 size = bullet.direction.x != 0.f ? glm::vec2(10.f, 6.f) : glm::vec2(6.f, 10.f);
        m_shell->setSize(size);
        m_shell->setPosition(glm::vec2(bullet.position.x, bullet.position.y) - size * 0.5f);
        m_shell->render();
    }
    shader->setInt("enemyTint", 0);
    if (m_lastHealth != m_world.health() || m_lastEnemies != m_world.enemiesLeft()) {
        m_lastHealth = m_world.health(); m_lastEnemies = m_world.enemiesLeft();
        m_hud = makeScoreboard(shader,
            L"\u0411\u0420\u041e\u041d\u042f: " + std::to_wstring(m_lastHealth) +
            L" / 3    |    \u0412\u0420\u0410\u0413\u0418: " + std::to_wstring(m_lastEnemies) + L" / 5", L"", 360, 34);
        m_hud->setPosition({300.f,603.f});
    }
    if (m_hud) m_hud->render();
    if (m_world.state() != m_lastState) {
        m_lastState = m_world.state();
        m_result.reset();
        if (m_lastState != World::State::Playing) {
            const bool won = m_lastState == World::State::Won;
            m_result = makeScoreboard(shader,
                won ? L"\u041f\u041e\u0417\u0414\u0420\u0410\u0412\u041b\u042f\u0415\u041c!" : L"\u0422\u0410\u041d\u041a \u0423\u041d\u0418\u0427\u0422\u041e\u0416\u0415\u041d",
                won ? L"\u0412\u0441\u0435 5 \u0432\u0440\u0430\u0436\u0435\u0441\u043a\u0438\u0445 \u0442\u0430\u043d\u043a\u043e\u0432 \u043f\u043e\u0431\u0435\u0436\u0434\u0435\u043d\u044b." : L"\u041f\u043e\u043f\u0440\u043e\u0431\u0443\u0439 \u0435\u0449\u0451 \u0440\u0430\u0437!",
                640, 240, won);
            m_result->setPosition({160.f,200.f});
        }
    }
    if (m_result) m_result->render();
}

