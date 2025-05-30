#include"VertexBuffer.hpp"

namespace RendererEngine {

	VertexBuffer::VertexBuffer()
	: m_id(0){
		
	}
	VertexBuffer::~VertexBuffer() {

	}

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& VertexBuffer) noexcept {
		m_id = VertexBuffer.m_id;
		VertexBuffer.m_id = 0;
		return *this;
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& VertexBuffer) noexcept {
		m_id = VertexBuffer.m_id;
		VertexBuffer.m_id = 0;
	}

	void VertexBuffer::init(const void* data, const unsigned int size) {
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	void VertexBuffer::update(const void* data, const unsigned int size) const {

		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void VertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void VertexBuffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}