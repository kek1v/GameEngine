#include"Renderer.hpp"

namespace RendererEngine {
	void Renderer::draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shader) {
		shader.use();
		vertexArray.bind();
		indexBuffer.bind();
		glDrawElements(GL_TRIANGLES, indexBuffer.getCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::setClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}
}