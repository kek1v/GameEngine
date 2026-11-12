#pragma once

#include<glad/glad.h>

namespace RendererEngine {
	class IndexBuffer {
	public:
		IndexBuffer();
		~IndexBuffer();

		IndexBuffer(IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer& IndexBuffer) = delete;
		IndexBuffer& operator=(IndexBuffer&& IndexBuffer) noexcept;
		IndexBuffer(IndexBuffer&& IndexBuffer) noexcept;

		void init(const void* data, const unsigned int count);
		void bind() const;
		void unbind() const;
		unsigned int getCount() const { return m_count; };

	private:
		GLuint m_id;
		unsigned int m_count;
	};
}