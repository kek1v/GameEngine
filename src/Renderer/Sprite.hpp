#pragma once

#include <glad/glad.h>
#include"VertexBuffer.hpp"
#include"IndexBuffer.hpp"
#include"VertexArray.hpp"
#include <vec2.hpp>

#include <memory>
#include <string>

namespace RendererEngine {
	class Texture2D;
	class ShaderProgram;

	class Sprite {
	public:
		Sprite(std::shared_ptr<Texture2D> pTexture, 
			   std::string initialSubTexture,
			   std::shared_ptr<ShaderProgram> pShaderprogram, 
			   const glm::vec2& position = glm::vec2(0.f), 
			   const glm::vec2& size = glm::vec2(1.f),
			   const float rotation = 0.f);


		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;

		virtual void render() const;
		void setPosition(const glm::vec2& position);
		void setSize(const glm::vec2& size);
		void setRotation(const float rotation);

	protected:
		std::shared_ptr<Texture2D> m_pTexture;
		std::shared_ptr<ShaderProgram> m_pShaderprogram;
		glm::vec2 m_size;
		glm::vec2 m_position;
		float m_rotation;

		VertexArray m_vertexArray;
		VertexBuffer m_vertexCoordsBuffer;
		VertexBuffer m_textureCoordsBuffer;
		IndexBuffer	 m_indexBuffer;
	};
}