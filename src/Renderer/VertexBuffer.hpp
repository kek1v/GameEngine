#pragma once

#include<glad/glad.h>

namespace Renderer {
	class VertexBuffer {
	public:
		VertexBuffer();
		~VertexBuffer();

		VertexBuffer(VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer& VertexBuffer) = delete;
		VertexBuffer& operator=(VertexBuffer&& VertexBuffer) noexcept;
		VertexBuffer(VertexBuffer&& VertexBuffer) noexcept;

		void init(const void* data, const unsigned int size);
		void update(const void* data, const unsigned int size) const;
		void bind() const;
		void unbind() const;

	private:
		GLuint m_id;
	};
}