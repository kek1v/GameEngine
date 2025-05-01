#include "AnimatedSprite.hpp"

#include <string>
#include <iostream>

namespace Renderer {

	AnimatedSprite::AnimatedSprite(const std::shared_ptr<Texture2D> pTexture,
		const std::string& initialSubTexture,
		const std::shared_ptr<ShaderProgram> pShaderprogram,
		const glm::vec2& position,
		const glm::vec2& size,
		const float rotation)
		: Sprite(std::move(pTexture), std::move(initialSubTexture), std::move(pShaderprogram), position, size, rotation) {


	}

	void AnimatedSprite::insertState(std::string stateName, std::vector<std::pair<std::string, uint64_t>> subTexturesDuration) {
		m_statesMap.emplace(std::move(stateName), std::move(subTexturesDuration));
	}

	void AnimatedSprite::render() const {

	}

	void AnimatedSprite::setState(const std::string& newState) {
		auto it = m_statesMap.find(newState);
		if (it != m_statesMap.end()) {
			std::cerr << "Can't find animation state: " << newState << std::endl;
			return;
		}
		
		m_currentAnimationTime = 0;
		m_currentFrame = 0;

	}

	void AnimatedSprite::update(const size_t delta) {

	}
}
