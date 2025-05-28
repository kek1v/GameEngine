#pragma once

#include"VertexArray.hpp"
#include"ShaderProgram.hpp"
#include"IndexBuffer.hpp"

#include <string>

namespace RendererEngine {
	class Renderer {
	public:
		static void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shader);
		static void setClearColor(float r, float g, float b, float a);
		static void clear();
		static void setViewport(unsigned int width, unsigned int height, unsigned int leftOffset = 0, unsigned int bottomOffset = 0);
		static std::string getRendererStr();
		static std::string getVersionStr();
	private:

	};
}