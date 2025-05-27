#include "Tank.hpp"
#include"../Renderer/AnimatedSprite.hpp"

Tank::Tank(std::shared_ptr<RendererEngine::AnimatedSprite> pSprite, const float velocity, const glm::vec2& position)
	: m_eOrientation(eOrientation::Top)
	, m_pSprite(std::move(pSprite))
	, m_move(false)
	, m_velocity(velocity)
	, m_moveOffset(glm::vec2(0.f, 1.f))
	, m_position(position){

	m_pSprite->setPosition(m_position);
}

void Tank::render() const {
	m_pSprite->render();
}

void Tank::setOrientation(const eOrientation eOrientation) {
	if (m_eOrientation == eOrientation) {
		return;
	}

	m_eOrientation = eOrientation;
	switch (m_eOrientation){
	case Tank::eOrientation::Top:
		m_pSprite->setState("tankTopState");
		m_moveOffset.x = 0.f;
		m_moveOffset.y = 1.f;
		break;
	case Tank::eOrientation::Bottom:
		m_pSprite->setState("tankBottomState");
		m_moveOffset.x = 0.f;
		m_moveOffset.y = -1.f;
		break;
	case Tank::eOrientation::left:
		m_pSprite->setState("tankLeftState");
		m_moveOffset.x = -1.f;
		m_moveOffset.y = 0.f;
		break;
	case Tank::eOrientation::right:
		m_pSprite->setState("tankRightState");
		m_moveOffset.x = 1.f;
		m_moveOffset.y = 0.f;
		break;
	default:
		break;
	}
}

void Tank::move(const bool move) {
	m_move = move;
}

void Tank::update(const uint64_t delta) {
	if (m_move) {
		m_position += delta * m_velocity * m_moveOffset;
		m_pSprite->setPosition(m_position);
		m_pSprite->update(delta);
	}
}