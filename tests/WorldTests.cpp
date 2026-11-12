#include "Game/World.hpp"
#include <cstdlib>
#include <iostream>
#include <cmath>

struct WorldTestAccess {
    static void combatSetup(World& world) {
        world.reset();
        for (auto& enemy : world.m_enemies) { enemy.cooldown = 1000.f; enemy.think = 1000.f; }
    }
    static void placeEnemy(World& world, int index, World::Point position) {
        auto& enemy = world.m_enemies[index];
        enemy.position = position; enemy.target = position;
    }
    static void shell(World& world, World::Point position, bool hostile) {
        world.m_bullets.push_back({position, {0.f, 0.f}, hostile});
    }
    static void readyAI(World& world, int index) { world.m_enemies[index].cooldown = 0.f; }
    static bool seesPlayer(World& world, int index) {
        World::Point direction;
        return world.canSeePlayer(world.m_enemies[index], direction);
    }
};

void check(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
void advance(World& world, int frames, int x, int y, bool fire = false) {
    for (int i = 0; i < frames; ++i) world.update(1.f / 60.f, x, y, fire);
}
int main() {
    World world;
    check(!world.blocked(world.tank(), World::TankSize), "spawn is clear");
    check(world.blocked({-1.f, 64.f}, World::TankSize), "outside map is solid");
    check(world.blocked({192.f, 96.f}, World::TankSize), "wall blocks tank");
    advance(world, 180, 0, 1);
    check(world.tank().y <= 136.001f && world.tank().y > 133.f, "tank stops at crate");
    world.reset();
    world.update(1.f / 60.f, 0, 0, true);
    check(world.bullets().size() == 1, "space fires a shell");
    advance(world, 5, 0, 0, true);
    check(world.bullets().size() == 1, "fire cooldown limits rate");
    advance(world, 30, 0, 0);
    check(world.tile(2, 5) == World::Tile::Floor, "shell destroys crate");
    check(world.destroyedCrates() == 1 && world.bullets().empty(), "shell consumed on hit");
    advance(world, 80, 0, 1);
    check(world.tank().y > 160.f, "destroyed crate is passable");
    world.reset();
    advance(world, 1, -1, 0);
    advance(world, 120, 0, 0, true);
    check(world.tile(0, 2) == World::Tile::Wall, "wall survives shells");
    check(world.destroyedCrates() == 0 && world.bullets().size() <= 1, "wall absorbs shells");
    advance(world, 180, -1, 0);
    check(world.tank().x >= 32.f && world.tank().x < 35.f, "border blocks movement");
    world.reset();
    world.update(10.f, 0, 1, true);
    check(!world.blocked(world.tank(), World::TankSize), "long frame cannot tunnel");
    world.reset();
    check(world.tile(2,5) == World::Tile::Crate && world.bullets().empty(), "restart restores map");
    check(world.enemiesLeft() == 5 && world.health() == 3, "five enemies and full armor");
    for (const auto& enemy : world.enemies())
        check(!world.blocked(enemy.position, World::TankSize), "enemy spawn is clear");
    const auto firstPosition = world.enemies()[0].position;
    advance(world, 60, 0, 0);
    check(std::abs(world.enemies()[0].position.x - firstPosition.x) +
          std::abs(world.enemies()[0].position.y - firstPosition.y) > 10.f, "AI pursues player");
    for (const auto& enemy : world.enemies())
        check(!world.blocked(enemy.position, World::TankSize), "AI respects terrain");

    WorldTestAccess::combatSetup(world);
    WorldTestAccess::placeEnemy(world, 0, {132.f, 68.f});
    check(WorldTestAccess::seesPlayer(world, 0), "AI sees player across clear corridor");
    WorldTestAccess::readyAI(world, 0);
    advance(world, 1, 0, 0);
    check(!world.bullets().empty() && world.bullets()[0].hostile, "AI fires hostile shell");
    advance(world, 20, 0, 0);
    check(world.health() == 2, "enemy shell damages player");

    WorldTestAccess::combatSetup(world);
    WorldTestAccess::placeEnemy(world, 0, {68.f, 228.f});
    check(!WorldTestAccess::seesPlayer(world, 0), "crate blocks AI line of sight");
    WorldTestAccess::placeEnemy(world, 0, {132.f, 68.f});
    advance(world, 20, 1, 0, true);
    check(world.enemiesLeft() == 4, "player shell destroys enemy");
    for (int i = 1; i < World::EnemyCount; ++i) {
        const auto p = world.enemies()[i].position;
        WorldTestAccess::shell(world, {p.x + 12.f, p.y + 12.f}, false);
        advance(world, 1, 0, 0);
        check(world.enemiesLeft() == 4-i, "each enemy counted once");
        if (i < 4) check(world.state() == World::State::Playing, "no early victory");
    }
    check(world.state() == World::State::Won && world.bullets().empty(), "fifth kill wins and clears shells");
    const auto victoryPosition = world.tank();
    advance(world, 60, 1, 0, true);
    check(world.tank().x == victoryPosition.x && world.bullets().empty(), "victory freezes combat");
    WorldTestAccess::combatSetup(world);
    for (int hp = 2; hp >= 0; --hp) {
        const auto p = world.tank();
        WorldTestAccess::shell(world, {p.x+12.f,p.y+12.f}, true);
        advance(world, 1, 0, 0);
        check(world.health() == hp, "hostile hit reduces armor");
        advance(world, 65, 0, 0);
    }
    check(world.state() == World::State::Lost, "zero armor ends round");
    world.reset();
    check(world.state() == World::State::Playing && world.enemiesLeft() == 5 && world.health() == 3,
          "restart restores enemies and round state");
    std::cout << "PASS: spawn, collisions, shooting, cooldown, destructible crates, walls, reset\n";
}
