#include "compileShaders.h"
#include <iostream>
#include <string>
#include <vector>

ShaderLoader::ShaderLoader(bool tess) : tesselate(tess) {}

void ShaderLoader::loadShader(const char* vertexpath, const char* fragpath,const char* tc_path , const char* tes_path)
{
	
	std::string vertSource(readSource(vertexpath));
	std::string fragSource(readSource(fragpath));
	
	program = glCreateProgram();
	v_shader = glCreateShader(GL_VERTEX_SHADER);
	f_shader = glCreateShader(GL_FRAGMENT_SHADER);
		
	const char* vert_c = vertSource.c_str();
	const char* frag_c = fragSource.c_str();

	glShaderSource(v_shader, 1, &vert_c ,nullptr);
	glCompileShader(v_shader);
	checkCompileStatus(v_shader);

	
	glShaderSource(f_shader, 1, &frag_c, nullptr);
	glCompileShader(f_shader);
	checkCompileStatus(f_shader);
	
	glAttachShader(program, v_shader);
	glAttachShader(program, f_shader);

	if (tesselate)
	{
		std::string tcSource(readSource(tc_path));
		std::string tesSource(readSource(tes_path));
		tc_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
		tes_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);

		const char* tcs_c = tcSource.c_str();
		const char* tes_c = tesSource.c_str();

		glShaderSource(tc_shader, 1, &tcs_c, nullptr);
		glCompileShader(tc_shader);
		checkCompileStatus(tc_shader);

		glShaderSource(tes_shader, 1, &tes_c, nullptr);
		glCompileShader(tes_shader);
		checkCompileStatus(tes_shader);


		glAttachShader(program, tc_shader);
		glAttachShader(program, tes_shader);

	}

	glLinkProgram(program);
	GLint isLinked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);

	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

		glDeleteProgram(program);
		glDeleteShader(v_shader);
		glDeleteShader(f_shader);
		if (tesselate) {
			glDeleteShader(tc_shader);
			glDeleteShader(tes_shader);
			
		}
		std::printf("%s\n", &(errorLog[0]));
	}



	glDetachShader(program, v_shader);
	glDetachShader(program, f_shader);
	glDetachShader(program, tc_shader);
	glDetachShader(program, tes_shader);

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);
	glDeleteShader(tc_shader);
	glDeleteShader(tes_shader);

	
}

char* ShaderLoader::readSource(const char* path) {
	
	FILE* fp;
	fopen_s(&fp, path, "rb");

	if (fp == NULL)
	{
		printf("fopen could not read file"); return NULL;
	} 
	
	
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	GLuint fSize = (GLuint)size;

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[fSize + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';
	fclose(fp);

	return buf;

}

void ShaderLoader::checkCompileStatus(GLuint ID)
{
	GLint success = 0;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {

		GLint maxLength = 0;
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(ID, maxLength, &maxLength, &errorLog[0]);

		std::printf("%s \n", &(errorLog[0]));
	}
}

void ShaderLoader::enableTess(bool value) 
{
	tesselate = value;
}







