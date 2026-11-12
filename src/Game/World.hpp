#pragma once
#include <array>
#include <vector>

// Game rules are independent of OpenGL so collisions can be tested headlessly.
class World {
public:
    static constexpr int Columns = 30, Rows = 20, TileSize = 32;
    static constexpr float TankSize = 24.f;
    enum class Tile { Floor, Wall, Crate };
    struct Point { float x, y; };
    struct Bullet { Point position, direction; bool hostile = false; };
    struct Enemy {
        Point position, direction{0.f, -1.f}, target{};
        float think = 0.f, cooldown = 1.f;
        bool alive = true;
    };
    enum class State { Playing, Won, Lost };
    static constexpr int EnemyCount = 5;
    const std::array<Enemy, EnemyCount>& enemies() const { return m_enemies; }
    int enemiesLeft() const;
    int health() const { return m_health; }
    State state() const { return m_state; }

    World();
    void reset();
    Tile tile(int x, int y) const;
    bool blocked(Point position, float size) const;
    void update(float seconds, int horizontal, int vertical, bool firing);
    const std::vector<Bullet>& bullets() const { return m_bullets; }
    Point tank() const { return m_tank; }
    Point direction() const { return m_direction; }
    bool moving() const { return m_moving; }
    int destroyedCrates() const { return m_destroyed; }
private:
    friend struct WorldTestAccess;
    bool occupied(Point position, int ignoreEnemy = -1) const;
    bool hitTank(const Bullet& bullet);
    void fire(Point position, Point direction, bool hostile);
    void updateEnemies(float seconds);
    Point nextWaypoint(int enemyIndex) const;
    bool canSeePlayer(const Enemy& enemy, Point& direction) const;
    std::array<Enemy, EnemyCount> m_enemies{};
    State m_state = State::Playing;
    int m_health = 3;
    float m_invulnerability = 0.f;
    bool hit(Point position);
    void step(float seconds, int horizontal, int vertical, bool firing);
    std::array<Tile, Columns * Rows> m_tiles{};
    std::vector<Bullet> m_bullets;
    Point m_tank{}, m_direction{};
    float m_cooldown = 0.f;
    int m_destroyed = 0;
    bool m_moving = false;
};
