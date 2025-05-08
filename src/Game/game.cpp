#include "game.hpp"


game::game() : m_eCurrentGameState(EGameState::Active)

{
	m_Keys.fill(false);
}

game::~game() {

}

void game::render() {

}

void game::update(const uint64_t delta) {

}

void game::setKey(const int key, const int action) {
	m_Keys[key] = action;
}

bool game::init() {
	return false;
}

