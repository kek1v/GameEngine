#pragma once
#include <cstdint>
#include <array>

class game {
public:
	game();
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

	EGameState m_eCurrentGameState;
};