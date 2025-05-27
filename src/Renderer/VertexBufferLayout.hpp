#pragma once

#include<vector>
#include<glad/glad.h>

namespace RendererEngine {

	struct vertexBufferLayoutElement {
		GLuint count;
		GLenum type;
		GLboolean normalized;
		GLint size;
	};

	class VertexBufferLayout {
	public:
		VertexBufferLayout();

		void reserveElements(const size_t count);
		const std::vector<vertexBufferLayoutElement>& getLayoutElements() const { return m_layoutElements; };
		void addElementLayoutFloat(const unsigned int count, const bool normalized);
		unsigned int getStride() const { return m_stride; };
	private:
		std::vector<vertexBufferLayoutElement> m_layoutElements;// layout - расположение
		unsigned int m_stride; // stride - шаг 
	};
}