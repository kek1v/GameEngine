#pragma once
#include <cstdint>
#include <array>
#include <vec2.hpp>

class game {
public:
	game(const glm::ivec2& windowSize);
	~game();

	void render();
	void update(const uint64_t delta);
	void setKey(const int key, const int action);
	bool init();

private:
	std::array<bool, 349> m_Keys;

	enum class EGameState {
		Active,
		Pause
	};
	
	const glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameState;
};