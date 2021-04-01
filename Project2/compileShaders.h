#pragma once
#include<GL/glew.h>

#define TESSELATION_ON 0x01

class ShaderLoader {

public:
	ShaderLoader(bool tess = 0);


	void loadShader(const char* vertexpath, const char* fragpath, const char* tc_path = nullptr, const char* tes_path = nullptr);
	char* readSource(const char* path);
	void checkCompileStatus(GLuint ID);
	void enableTess(bool value);

	GLuint program;
	
private:

	bool tesselate;

	

	GLuint v_shader;
	GLuint tc_shader;
	GLuint tes_shader;
	GLuint f_shader;



};




