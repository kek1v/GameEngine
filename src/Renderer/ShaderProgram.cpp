#include "ShaderProgram.hpp"
#include <gtc/type_ptr.hpp>


#include <iostream>

namespace RendererEngine {

	// ����������� ������ ShaderProgram, ������� ��������� ��������� ���������� � ������������ ��������
	ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {

		// ������� ���������� ��� �������� ID ���������� �������
		GLuint vertexShaderID;
		// ������� ������� � �������������� ��������� ������.
		// ���� ���������� �� �������, ������� ��������� �� ������ � ��������� ���������� ������������.
		if (!createShader(vertexShader, GL_VERTEX_SHADER, vertexShaderID)) {
			std::cerr << "VERTEX SHADER compile time error" << std::endl;
			return; 		}

		// ������� ���������� ��� �������� ID ������������ �������
		GLuint fragmentShaderID;
		// ���������� ������� � ����������� ����������� ������.
		if (!createShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderID)) {
			std::cerr << "FRAGMENT SHADER compile time error" << std::endl;
			glDeleteShader(fragmentShaderID); 		
			return;
		}

		// ������� ������ ��������� ��������
		m_ID = glCreateProgram();
		// ����������� ��������� � ����������� ������� � ���������
		glAttachShader(m_ID, vertexShaderID);
		glAttachShader(m_ID, fragmentShaderID);
		// ������� ���������, ����� ������� ������� ������
		glLinkProgram(m_ID); 
		GLint success;
		// ���������, ������� �� ������ �������� ���������
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success) {
			// ���� �������� �� �������, �������� ��� ������ � ������� ���
			GLchar infolog[1024];
			glGetShaderInfoLog(m_ID, 1024, nullptr, infolog); 
			std::cerr << "ERROR::SHADER: link time error\n" << infolog << std::endl;
		}
		else {
			// ���� �������� �������, ��������, ��� ��������� ��������������
			m_isCompiled = true;
		}

		// ������� �������, ��� ��� ��� ��� ����������� � ���������
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}


	// ������� ��� �������� � ���������� ���������� �������
	bool ShaderProgram::createShader(const std::string& source, const GLenum shaderType, GLuint& shaderID) {
		// ������� ������ ���������� ���� (��������� ��� �����������)
		shaderID = glCreateShader(shaderType);
		// �������� ��������� �� ������ � �������� ����� �������
		const char* code = source.c_str();
		// �������� �������� ��� � ������ �������
		glShaderSource(shaderID, 1, &code, nullptr);
		// ����������� ������
		glCompileShader(shaderID);

		GLint success;
		// ���������, ������� �� ������ ���������� �������
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success) {
			// ���� ��������� ������ ����������, �������� ��� ������ � ������� ���
			GLchar infolog[1024];
			glGetShaderInfoLog(shaderID, 1024, nullptr, infolog);
			std::cerr << "ERROR::SHADER: Compile time error\n" << infolog << std::endl;
			// ��� ������� ����� ���� ����������, ��� �������� ��� ������ � ��� ��� ����������������
			return false;
		}
		return true;
	}

	// ���������� ������, ������� ����������� ������� ��������� ��������
	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(m_ID);
	}

	// ������� ��� ������������� (���������) ������ ��������� ��������
	void ShaderProgram::use() const {
		glUseProgram(m_ID);
	}

	// ���������� ��������� ������������ ��� ����������� (move assignment)
	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) noexcept {
		// ������� ������� ��������� ��������, ����� �� ���� ������ ������
		glDeleteProgram(m_ID);
		// ��������� ID � ���� ���������� �� ����������� �������
		m_ID = shaderProgram.m_ID;
		m_isCompiled = shaderProgram.m_isCompiled;

		// �������� �������� � ���������� �������, ����� �������� �������� ��������
		shaderProgram.m_ID = 0;
		shaderProgram.m_isCompiled = false;
		return *this;
	}

	// ����������� ����������� (move constructor)
	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept {
		// ��������� ������ �� ����������� �������
		m_ID = shaderProgram.m_ID;
		m_isCompiled = shaderProgram.m_isCompiled;

		// �������� ������ � ���������� �������, ����� �� �� ��� ���������� ������� ����� �����������
		shaderProgram.m_ID = 0;
		shaderProgram.m_isCompiled = false;
	}

	void ShaderProgram::setInt(const std::string& name, const GLint value) {
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	
	void ShaderProgram::setMatrix4(const std::string& name, const glm::mat4& matrix) {
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

}
