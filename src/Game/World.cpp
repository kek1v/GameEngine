#include "World.hpp"
#include <algorithm>
#include <cmath>
#include <queue>

World::World() { reset(); }

void World::reset() {
    m_tiles.fill(Tile::Floor);
    auto put = [this](int x, int y, Tile value) { m_tiles[y * Columns + x] = value; };
    for (int x = 0; x < Columns; ++x) { put(x, 0, Tile::Wall); put(x, Rows - 1, Tile::Wall); }
    for (int y = 0; y < Rows; ++y) { put(0, y, Tile::Wall); put(Columns - 1, y, Tile::Wall); }
    // Broken walls leave several routes between the courtyard sections.
    for (int x : {6, 14, 22})
        for (int y = 3; y <= 16; ++y)
            if (y != 6 && y != 7 && y != 12 && y != 13) put(x, y, Tile::Wall);
    for (int x = 3; x < 27; ++x)
        if ((x >= 8 && x <= 11) || (x >= 16 && x <= 19) || x >= 24) put(x, 10, Tile::Wall);
    for (auto p : {Point{2,5}, {3,5}, {4,8}, {3,14}, {4,15}, {8,3}, {9,3},
                   {10,7}, {11,7}, {8,14}, {10,16}, {12,15}, {16,3}, {17,4},
                   {19,7}, {20,7}, {16,13}, {18,15}, {19,15}, {24,3}, {25,3},
                   {26,6}, {24,13}, {25,14}, {27,16}, {12,5}, {20,17}})
        put(static_cast<int>(p.x), static_cast<int>(p.y), Tile::Crate);
    m_tank = {2.f * TileSize + 4.f, 2.f * TileSize + 4.f};
    m_direction = {0.f, 1.f};
    m_bullets.clear();
    m_cooldown = 0.f;
    m_destroyed = 0;
    m_moving = false;
    m_state = State::Playing;
    m_health = 3;
    m_invulnerability = 0.f;
    const Point spawns[EnemyCount] = {{26,17}, {18,17}, {10,17}, {26,8}, {18,5}};
    for (int i = 0; i < EnemyCount; ++i) {
        m_enemies[i] = Enemy{};
        m_enemies[i].position = {spawns[i].x * TileSize + 4.f, spawns[i].y * TileSize + 4.f};
        m_enemies[i].target = m_enemies[i].position;
        m_enemies[i].cooldown = 1.f + i * 0.23f;
    }
}

World::Tile World::tile(int x, int y) const {
    if (x < 0 || y < 0 || x >= Columns || y >= Rows) return Tile::Wall;
    return m_tiles[y * Columns + x];
}

bool World::blocked(Point position, float size) const {
    const int left = static_cast<int>(std::floor(position.x / TileSize));
    const int bottom = static_cast<int>(std::floor(position.y / TileSize));
    const int right = static_cast<int>(std::floor((position.x + size - 0.001f) / TileSize));
    const int top = static_cast<int>(std::floor((position.y + size - 0.001f) / TileSize));
    for (int y = bottom; y <= top; ++y)
        for (int x = left; x <= right; ++x)
            if (tile(x, y) != Tile::Floor) return true;
    return false;
}

bool World::hit(Point position) {
    constexpr float radius = 3.f;
    const int left = static_cast<int>(std::floor((position.x - radius) / TileSize));
    const int bottom = static_cast<int>(std::floor((position.y - radius) / TileSize));
    const int right = static_cast<int>(std::floor((position.x + radius) / TileSize));
    const int top = static_cast<int>(std::floor((position.y + radius) / TileSize));
    // A wall shields any crate adjacent to it.
    for (int y = bottom; y <= top; ++y)
        for (int x = left; x <= right; ++x)
            if (tile(x, y) == Tile::Wall) return true;
    for (int y = bottom; y <= top; ++y)
        for (int x = left; x <= right; ++x)
            if (tile(x, y) == Tile::Crate) {
                m_tiles[y * Columns + x] = Tile::Floor;
                ++m_destroyed;
                return true;
            }
    return false;
}

void World::update(float seconds, int horizontal, int vertical, bool firing) {
    if (seconds <= 0.f || m_state != State::Playing) return;
    // Small steps prevent fast shells from skipping obstacles, even on a slow frame.
    seconds = std::min(seconds, 0.1f);
    m_moving = false;
    while (seconds > 0.f && m_state == State::Playing) {
        const float dt = std::min(seconds, 1.f / 240.f);
        step(dt, horizontal, vertical, firing);
        seconds -= dt;
    }
}

void World::step(float seconds, int horizontal, int vertical, bool firing) {
    if (horizontal != 0 || vertical != 0) {
        m_direction = horizontal != 0 ? Point{horizontal > 0 ? 1.f : -1.f, 0.f}
                                     : Point{0.f, vertical > 0 ? 1.f : -1.f};
        Point next{m_tank.x + m_direction.x * 125.f * seconds,
                   m_tank.y + m_direction.y * 125.f * seconds};
        if (!blocked(next, TankSize) && !occupied(next)) { m_tank = next; m_moving = true; }
    }
    m_cooldown = std::max(0.f, m_cooldown - seconds);
    if (firing && m_cooldown <= 0.f) {
        fire(m_tank, m_direction, false);
        m_cooldown = 0.25f;
    }
    m_invulnerability = std::max(0.f, m_invulnerability - seconds);
    updateEnemies(seconds);
    for (auto it = m_bullets.begin(); it != m_bullets.end();) {
        it->position.x += it->direction.x * 420.f * seconds;
        it->position.y += it->direction.y * 420.f * seconds;
        if (hit(it->position) || hitTank(*it)) it = m_bullets.erase(it);
        else ++it;
    }
    if (m_health <= 0) m_state = State::Lost;
    else if (enemiesLeft() == 0) m_state = State::Won;
    if (m_state != State::Playing) m_bullets.clear();
}

namespace {
bool overlaps(World::Point a, World::Point b, float size = World::TankSize) {
    return a.x < b.x + size && a.x + World::TankSize > b.x &&
           a.y < b.y + size && a.y + World::TankSize > b.y;
}
bool shellTouches(World::Point p, World::Point tank) {
    return p.x + 3.f >= tank.x && p.x - 3.f <= tank.x + World::TankSize &&
           p.y + 3.f >= tank.y && p.y - 3.f <= tank.y + World::TankSize;
}
}

int World::enemiesLeft() const {
    return static_cast<int>(std::count_if(m_enemies.begin(), m_enemies.end(), [](const Enemy& e) { return e.alive; }));
}

bool World::occupied(Point position, int ignoreEnemy) const {
    if (ignoreEnemy >= 0 && overlaps(position, m_tank)) return true;
    for (int i = 0; i < EnemyCount; ++i)
        if (i != ignoreEnemy && m_enemies[i].alive && overlaps(position, m_enemies[i].position)) return true;
    return false;
}

bool World::hitTank(const Bullet& bullet) {
    if (bullet.hostile) {
        if (!shellTouches(bullet.position, m_tank)) return false;
        if (m_invulnerability <= 0.f) { --m_health; m_invulnerability = 1.f; }
        return true;
    }
    for (auto& enemy : m_enemies)
        if (enemy.alive && shellTouches(bullet.position, enemy.position)) {
            enemy.alive = false;
            return true;
        }
    return false;
}

void World::fire(Point position, Point direction, bool hostile) {
    Point muzzle{position.x + TankSize / 2.f + direction.x * (TankSize / 2.f + 3.f),
                 position.y + TankSize / 2.f + direction.y * (TankSize / 2.f + 3.f)};
    Bullet bullet{muzzle, direction, hostile};
    if (!hit(muzzle) && !hitTank(bullet)) m_bullets.push_back(bullet);
}

bool World::canSeePlayer(const Enemy& enemy, Point& direction) const {
    const float dx = m_tank.x - enemy.position.x, dy = m_tank.y - enemy.position.y;
    if (std::abs(dx) < 8.f) direction = {0.f, dy > 0 ? 1.f : -1.f};
    else if (std::abs(dy) < 8.f) direction = {dx > 0 ? 1.f : -1.f, 0.f};
    else return false;
    const float distance = std::max(std::abs(dx), std::abs(dy));
    for (float t = 16.f; t < distance; t += 4.f) {
        Point p{enemy.position.x + TankSize / 2.f + direction.x * t,
                enemy.position.y + TankSize / 2.f + direction.y * t};
        if (tile(int(p.x / TileSize), int(p.y / TileSize)) != Tile::Floor) return false;
        for (const auto& other : m_enemies)
            if (&other != &enemy && other.alive && shellTouches(p, other.position)) return false;
    }
    return true;
}

World::Point World::nextWaypoint(int enemyIndex) const {
    const auto startPosition = m_enemies[enemyIndex].position;
    const int sx = int((startPosition.x + TankSize / 2.f) / TileSize);
    const int sy = int((startPosition.y + TankSize / 2.f) / TileSize);
    const int gx = int((m_tank.x + TankSize / 2.f) / TileSize);
    const int gy = int((m_tank.y + TankSize / 2.f) / TileSize);
    const int start = sy * Columns + sx, goal = gy * Columns + gx;
    std::array<int, Columns * Rows> previous;
    previous.fill(-1);
    previous[start] = start;
    std::queue<int> pending;
    pending.push(start);
    const int offsets[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    while (!pending.empty() && previous[goal] == -1) {
        const int cell = pending.front(); pending.pop();
        for (const auto& d : offsets) {
            const int x = cell % Columns + d[0], y = cell / Columns + d[1];
            if (tile(x,y) != Tile::Floor) continue;
            const int next = y * Columns + x;
            if (previous[next] != -1) continue;
            bool taken = false;
            for (int j = 0; j < EnemyCount; ++j)
                if (j != enemyIndex && m_enemies[j].alive &&
                    int((m_enemies[j].position.x + 12.f) / TileSize) == x &&
                    int((m_enemies[j].position.y + 12.f) / TileSize) == y) taken = true;
            if (taken) continue;
            previous[next] = cell;
            pending.push(next);
        }
    }
    if (previous[goal] == -1 || start == goal) return startPosition;
    int next = goal;
    while (previous[next] != start) next = previous[next];
    return {float((next % Columns) * TileSize + 4), float((next / Columns) * TileSize + 4)};
}

void World::updateEnemies(float seconds) {
    for (int i = 0; i < EnemyCount; ++i) {
        auto& enemy = m_enemies[i];
        if (!enemy.alive) continue;
        enemy.cooldown = std::max(0.f, enemy.cooldown - seconds);
        Point aim;
        if (canSeePlayer(enemy, aim)) {
            enemy.direction = aim;
            if (enemy.cooldown <= 0.f) { fire(enemy.position, aim, true); enemy.cooldown = 1.4f; }
            continue;
        }
        enemy.think -= seconds;
        const float dx = enemy.target.x - enemy.position.x, dy = enemy.target.y - enemy.position.y;
        if (std::abs(dx) < 0.01f && std::abs(dy) < 0.01f) {
            if (enemy.think > 0.f) continue;
            enemy.target = nextWaypoint(i);
            enemy.think = 0.15f;
        }
        const float tx = enemy.target.x - enemy.position.x, ty = enemy.target.y - enemy.position.y;
        // Finish one axis before turning, keeping the hull inside narrow passages.
        Point next = enemy.position;
        if (std::abs(tx) > 0.01f) {
            enemy.direction = {tx > 0.f ? 1.f : -1.f, 0.f};
            next.x += enemy.direction.x * std::min(std::abs(tx), 65.f * seconds);
        } else if (std::abs(ty) > 0.01f) {
            enemy.direction = {0.f, ty > 0.f ? 1.f : -1.f};
            next.y += enemy.direction.y * std::min(std::abs(ty), 65.f * seconds);
        }
        if (!blocked(next, TankSize) && !occupied(next, i)) enemy.position = next;
        else if (enemy.think <= 0.f) { enemy.target = enemy.position; enemy.think = 0.f; }
    }
}
