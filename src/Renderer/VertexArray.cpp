#include"VertexArray.hpp"
#include<memory>

namespace RendererEngine {

	VertexArray::VertexArray()
	{

		glGenVertexArrays(1, &m_id);
	}
	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_id);
	}

	VertexArray& VertexArray::operator=(VertexArray&& VertexArray) noexcept {
		m_id = VertexArray.m_id;
		VertexArray.m_id = 0;
		return *this;
	}

	VertexArray::VertexArray(VertexArray&& VertexArray) noexcept {
		m_id = VertexArray.m_id;
		VertexArray.m_id = 0;
	}

	void VertexArray::bind() const {
		glBindVertexArray(m_id);
	}

	void VertexArray::unbind() const {
		glBindVertexArray(0);
	}

	void VertexArray::addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout) {
		bind();
		vertexBuffer.bind();
		const auto& layoutElements = layout.getLayoutElements();
		GLbyte* offset = nullptr;
		for (unsigned int i = 0;i < layoutElements.size();++i) {
			const auto& currentlayoutElement = layoutElements[i];
			GLuint currentAttribIndex = m_elementsCount + i;
			glEnableVertexAttribArray(currentAttribIndex);
			glVertexAttribPointer(currentAttribIndex, currentlayoutElement.count, currentlayoutElement.type, currentlayoutElement.normalized, layout.getStride(), offset);
			offset += currentlayoutElement.size;
		}
		m_elementsCount += static_cast<unsigned int>(layoutElements.size());


	}
}