#pragma once

#include"Sprite.hpp"
#include <map>
#include <vector>

namespace Renderer {
	class AnimatedSprite : public Sprite{
	public:
		AnimatedSprite(const std::shared_ptr<Texture2D> pTexture,
			const std::string& initialSubTexture,
			const std::shared_ptr<ShaderProgram> pShaderprogram,
			const glm::vec2& position = glm::vec2(0.f),
			const glm::vec2& size = glm::vec2(1.f),
			const float rotation = 0.f);

		void insertState(std::string stateName, std::vector<std::pair<std::string, uint64_t>> subTexturesDuration);

		void render() const override;

		void setState(const std::string& newState);
		void update(const size_t delta); // delta - время  с прошлого кадра

	private:
		std::map<std::string, std::vector<std::pair<std::string, uint64_t>>> m_statesMap; // хранит состояния, последовательность кадров и их длительность
		size_t m_currentFrame = 0; 
		uint64_t m_currentAnimationTime = 0;
};
}