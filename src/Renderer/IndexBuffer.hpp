#pragma once

#include<glad/glad.h>

namespace Renderer {
	class IndexBuffer {
	public:
		IndexBuffer();
		~IndexBuffer();

		IndexBuffer(IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer& IndexBuffer) = delete;
		IndexBuffer& operator=(IndexBuffer&& IndexBuffer) noexcept;
		IndexBuffer(IndexBuffer&& IndexBuffer) noexcept;

		void init(const void* data, const unsigned int size);
		void bind() const;
		void unbind() const;

	private:
		GLuint m_id;
	};
}