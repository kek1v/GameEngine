#pragma once
#include<glad/glad.h>
#include"VertexBuffer.hpp"
#include"VertexBufferLayout.hpp"

namespace RendererEngine {
	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		VertexArray(VertexArray&) = delete;
		VertexArray& operator=(const VertexArray& VertexArray) = delete;
		VertexArray& operator=(VertexArray&& VertexArray) noexcept;
		VertexArray(VertexArray&& VertexArray) noexcept;

		void addBuffer(const VertexBuffer& vertexbuffer, const VertexBufferLayout& layout);
		void bind() const;
		void unbind() const;

	private:
		GLuint m_id = 0;
		unsigned int m_elementsCount = 0;
	};
}