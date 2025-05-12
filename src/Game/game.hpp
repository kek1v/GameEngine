#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include <vec2.hpp>

class Tank;

class game {
public:
	game(const glm::ivec2& windowSize);
	~game();

	void render();
	void update(const uint64_t delta);
	void setKey(const int key, const int action);
	bool init();

protected:
	std::array<bool, 349> m_Keys;

	enum class EGameState {
		Active,
		Pause
	};
	
	const glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameState;
	std::unique_ptr<Tank> m_pTank;
};