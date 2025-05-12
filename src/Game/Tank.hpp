#pragma once

#include<memory>
#include<vec2.hpp>

namespace Renderer {
	class AnimatedSprite;
}

class Tank {
public:

	enum class eOrientation {
		Top,
		Bottom,
		left,
		right
	};

	Tank(std::shared_ptr<Renderer::AnimatedSprite> pSprite, const float velocity, const glm::vec2& position);

	void render() const;
	void setOrientation(const eOrientation eOrientation);
	void move(const bool move);
	void update(const uint64_t delta);


private:
	eOrientation m_eOrientation;
	std::shared_ptr<Renderer::AnimatedSprite> m_pSprite;
	bool m_move;
	glm::vec2 m_position;
	float m_velocity;
	glm::vec2 m_moveOffset;
};