#include "ShaderProgram.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <GL/glew.h> 

ShaderProgram::ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file)
{
	std::vector<GLuint> shader_ids;
	shader_ids.push_back(compile_shader(VS_file, GL_VERTEX_SHADER));
	shader_ids.push_back(compile_shader(FS_file, GL_FRAGMENT_SHADER));
	
	ID = link_shader(shader_ids);
}

void ShaderProgram::activate(void)
{
	glUseProgram(ID);
}

void ShaderProgram::deactivate(void)
{
	glUseProgram(ID);
}

void ShaderProgram::clear(void) {
	deactivate();
	glDeleteProgram(ID);
	ID = 0;
}


inline void ShaderProgram::setUniform(const std::string& name, const float val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1f(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const int val)
{
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "Uniform '" << name << "' not found.\n";
		return;
	}
	glUniform1i(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3 val)
{
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "Uniform '" << name << "' not found.\n";
		return;
	}
	glUniform3fv(loc, 1, glm::value_ptr(val));
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::vec4 val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform4fv(loc, 1, glm::value_ptr(val));
}


void ShaderProgram::setUniform(const std::string& name, const glm::mat3 val)
{
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "Uniform '" << name << "' not found.\n";
		return;
	}
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::mat4 val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cout << "no uniform with name:" << name << '\n';
		return;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

std::string ShaderProgram::getShaderInfoLog(const GLuint obj) {
	int infologLength = 0;
	std::string s;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		std::vector<char> v(infologLength);
		glGetShaderInfoLog(obj, infologLength, NULL, v.data());
		s.assign(begin(v), end(v));
	}
	return s;
}


std::string ShaderProgram::getProgramInfoLog(const GLuint obj)
{
	int infologLength = 0;
	std::string s;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0) {
		std::vector<char> v(infologLength);
		glGetProgramInfoLog(obj, infologLength, NULL, v.data());
		s.assign(begin(v), end(v));
	}
	return s;
}

GLuint ShaderProgram::compile_shader(const std::filesystem::path& source_file, const GLenum type)
{
	GLuint shader_h = glCreateShader(type);
	std::string shader_string = textFileRead(source_file).c_str();
	const char* shader_cstring = shader_string.c_str();

	glShaderSource(shader_h, 1, &shader_cstring, NULL);
	glCompileShader(shader_h);
	{ // check compile result, display error (if any)
		GLint cmpl_status;
		glGetShaderiv(shader_h, GL_COMPILE_STATUS, &cmpl_status);
		if (cmpl_status == GL_FALSE) {
			std::cerr << getShaderInfoLog(shader_h);
			throw std::exception("Shader compilation err. \n");
		}
		return shader_h;
	}
}

GLuint ShaderProgram::link_shader(const std::vector<GLuint> shader_ids)
{
		GLuint prog_h = glCreateProgram();
		for (GLuint id : shader_ids) {
			glAttachShader(prog_h, id);
		}
		glLinkProgram(prog_h);
		{ // check link result, display error (if any)
			GLint status;
			glGetProgramiv(prog_h, GL_LINK_STATUS, &status);
			if (status == GL_FALSE) {
				std::cerr << getProgramInfoLog(prog_h);
				throw std::exception("Link err. \n");
			}
		}
		return prog_h;
}

std::string ShaderProgram::textFileRead(const std::filesystem::path& filename)
{
		std::ifstream file(filename);
		if (!file.is_open()) throw std::exception("Error opening file.\n");
		std::stringstream ss;
		ss << file.rdbuf();

		return ss.str();
}
