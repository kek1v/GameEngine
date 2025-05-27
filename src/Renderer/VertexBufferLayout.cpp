#include"VertexBufferLayout.hpp"
#include <vector>

namespace RendererEngine {
	VertexBufferLayout::VertexBufferLayout() 
		: m_stride(0)
	{

	}

	void VertexBufferLayout::reserveElements(const size_t count) {
		m_layoutElements.reserve(count); // выделение памяти 
	}

	void VertexBufferLayout::addElementLayoutFloat(const unsigned int count, const bool normalized) {
		m_layoutElements.push_back({ count, GL_FLOAT, normalized });
		m_stride += m_layoutElements.back().size;
	}
}
