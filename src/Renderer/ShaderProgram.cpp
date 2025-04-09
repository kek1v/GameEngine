#include "ShaderProgram.hpp"
#include <gtc/type_ptr.hpp>


#include <iostream>

namespace Renderer {

	// Конструктор класса ShaderProgram, который принимает исходники вершинного и фрагментного шейдеров
	ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {

		// Создаем переменную для хранения ID вершинного шейдера
		GLuint vertexShaderID;
		// Попытка создать и скомпилировать вершинный шейдер.
		// Если компиляция не удалась, выводим сообщение об ошибке и завершаем выполнение конструктора.
		if (!createShader(vertexShader, GL_VERTEX_SHADER, vertexShaderID)) {
			std::cerr << "VERTEX SHADER compile time error" << std::endl;
			return; 		}

		// Создаем переменную для хранения ID фрагментного шейдера
		GLuint fragmentShaderID;
		// Аналогично создаем и компилируем фрагментный шейдер.
		if (!createShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderID)) {
			std::cerr << "FRAGMENT SHADER compile time error" << std::endl;
			glDeleteShader(fragmentShaderID); 		
			return;
		}

		// Создаем объект программы шейдеров
		m_ID = glCreateProgram();
		// Прикрепляем вершинный и фрагментный шейдеры к программе
		glAttachShader(m_ID, vertexShaderID);
		glAttachShader(m_ID, fragmentShaderID);
		// Линкуем программу, чтобы связать шейдеры вместе
		glLinkProgram(m_ID); 
		GLint success;
		// Проверяем, успешно ли прошла линковка программы
		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success) {
			// Если линковка не удалась, получаем лог ошибок и выводим его
			GLchar infolog[1024];
			glGetShaderInfoLog(m_ID, 1024, nullptr, infolog); 
			std::cerr << "ERROR::SHADER: link time error\n" << infolog << std::endl;
		}
		else {
			// Если линковка успешна, отмечаем, что программа скомпилирована
			m_isCompiled = true;
		}

		// Удаляем шейдеры, так как они уже прикреплены к программе
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}


	// Функция для создания и компиляции отдельного шейдера
	bool ShaderProgram::createShader(const std::string& source, const GLenum shaderType, GLuint& shaderID) {
		// Создаем шейдер указанного типа (вершинный или фрагментный)
		shaderID = glCreateShader(shaderType);
		// Получаем указатель на строку с исходным кодом шейдера
		const char* code = source.c_str();
		// Передаем исходный код в объект шейдера
		glShaderSource(shaderID, 1, &code, nullptr);
		// Компилируем шейдер
		glCompileShader(shaderID);

		GLint success;
		// Проверяем, успешно ли прошла компиляция шейдера
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success) {
			// Если произошла ошибка компиляции, получаем лог ошибок и выводим его
			GLchar infolog[1024];
			glGetShaderInfoLog(shaderID, 1024, nullptr, infolog);
			std::cerr << "ERROR::SHADER: Compile time error\n" << infolog << std::endl;
			// Для новичка может быть неочевидно, что означает лог ошибок и как его интерпретировать
			return false;
		}
		return true;
	}

	// Деструктор класса, который освобождает ресурсы программы шейдеров
	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(m_ID);
	}

	// Функция для использования (активации) данной программы шейдеров
	void ShaderProgram::use() const {
		glUseProgram(m_ID);
	}

	// Перегрузка оператора присваивания для перемещения (move assignment)
	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) noexcept {
		// Удаляем текущую программу шейдеров, чтобы не было утечек памяти
		glDeleteProgram(m_ID);
		// Переносим ID и флаг компиляции из переданного объекта
		m_ID = shaderProgram.m_ID;
		m_isCompiled = shaderProgram.m_isCompiled;

		// Обнуляем значения в переданном объекте, чтобы избежать двойного удаления
		shaderProgram.m_ID = 0;
		shaderProgram.m_isCompiled = false;
		return *this;
	}

	// Конструктор перемещения (move constructor)
	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept {
		// Переносим данные из переданного объекта
		m_ID = shaderProgram.m_ID;
		m_isCompiled = shaderProgram.m_isCompiled;

		// Обнуляем данные в переданном объекте, чтобы он не мог уничтожать ресурсы после перемещения
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
