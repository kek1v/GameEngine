#pragma once

#include"VertexArray.hpp"
#include"ShaderProgram.hpp"
#include"IndexBuffer.hpp"

namespace RendererEngine {
	class Renderer {
	public:
		static void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shader);
		static void setClearColor(float r, float g, float b, float a);
	private:

	};
}