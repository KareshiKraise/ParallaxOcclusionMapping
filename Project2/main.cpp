#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLM/glm.hpp>
#include "compileShaders.h"
#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <algorithm>
#include "Mesh.h"


GLfloat red[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat gray[] = {0.2f , 0.2f, 0.2f ,1.0f};
ShaderLoader my_program;
GLuint vao;
GLuint vbo_pos;
GLuint vbo_ind;
GLsizei IndexCount;
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
int WDIM = 512;
float FOV = 60.0f;

glm::mat4 Model = glm::mat4(1.0f);

bool arcball = false;
GLfloat TessLevelInner = 1.0;
GLfloat TessLevelOuter = 1.0;
Mesh myModel;

glm::vec4 Ambient = { 0.15f, 0.15f, 0.15f, 1.0f };

glm::vec3 LightColor = {1.0f, 1.0f, 1.0f};
glm::vec3 LightDirection = {10.0f, 10.0f, -1.0f};
glm::vec3 CameraPos = { 30.0f, 7.0f, -10.0f };
float Shininess = 10.0f;
float Strength = 0.5f;

int has_relief = 1;


void mousewheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		FOV += 2.0f;
	}
	else
	{
		FOV -= 2.0f;
	}
	return;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 't':
	case 'T':
		if(TessLevelInner <= 7.0) TessLevelInner += 1.0;
		break;
	case 'r':
	case 'R':
		if (TessLevelOuter <= 7.0) TessLevelOuter += 1.0;
		break;
	case '+':
		FOV += 2.0f;
		break;
	case '-':
		FOV -= 2.0f;
		break;
	}
}

void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		arcball = true;
		last_mx = cur_mx = x;
		last_my = cur_my = y;
		//std::cout << "clicked the left button" << std::endl;
	}
	else {
		arcball = false;
	}

}

void onMotion(int x, int y) {

	if (arcball)
	{
		cur_mx = x;
		cur_my = y;

		//std::cout << "last x" << last_mx << "cur x" << cur_mx << std::endl;
		//std::cout << "last y" << last_my << "cur y" << cur_my << std::endl;
	}



}

glm::vec3 get_arcball_vector(int x, int y) {

	
	glm::vec3 P = glm::vec3(1.0 * x / WDIM * 2 - 1.0, 1.0*y / WDIM * 2 - 1.0, 0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1 * 1) {
		P.z = sqrt(1 * 1 - OP_squared);
	}
	else {
		P = glm::normalize(P);
	}
	return P;
}

void initshaders(void) {
	
   // my_program.enableTess(true);
	my_program.loadShader("test.vertex", "test.frag");

	

	//glPointSize(30.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

void make_cube(void) {
	 
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	
	initshaders();

};

void resize(int w, int h) {
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

void idlefunc() 
{
	//
	if (cur_mx != last_mx || cur_my != last_my)
	{
		glm::vec3 va = get_arcball_vector(last_mx, last_my);
		glm::vec3 vb = get_arcball_vector(cur_mx, cur_my);
		float angle = acos(std::min(1.0f, glm::dot(va, vb)));
		angle = angle*0.03;
		glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
		

		Model = glm::rotate(Model, glm::degrees(angle), axis_in_camera_coord);

		last_mx = cur_mx;
		last_my = cur_my;
	}
	glutPostRedisplay();

	
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(glm::radians(FOV), ((float)(WDIM) / (float)(WDIM)), 0.1f, 60.0f);
	//30, 7, -10
	glm::mat4 Camera = glm::lookAt(CameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 ModelView = Model * Camera;

	glm::mat3 NormalMatrix = glm::transpose(glm::inverse(glm::mat3(ModelView)));
							
	glUseProgram(my_program.program);
		
	GLuint MatrixID = glGetUniformLocation(my_program.program, "M");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Model[0][0]);
	
	MatrixID = glGetUniformLocation(my_program.program, "V");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Camera[0][0]);
	
	MatrixID = glGetUniformLocation(my_program.program, "P");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Projection[0][0]);

	MatrixID = glGetUniformLocation(my_program.program, "NormalMatrix");
	glUniformMatrix3fv(MatrixID, 1, GL_FALSE, &NormalMatrix[0][0]);

	GLuint uniID = glGetUniformLocation(my_program.program, "Ambient");
	glUniform4fv(uniID, 1, glm::value_ptr(Ambient));

	uniID = glGetUniformLocation(my_program.program, "LightColor");
	glUniform3fv(uniID, 1, glm::value_ptr(LightColor));

	uniID = glGetUniformLocation(my_program.program, "CameraPos");
	glUniform3fv(uniID, 1, glm::value_ptr(CameraPos));

	uniID = glGetUniformLocation(my_program.program, "LightDirection");
	glUniform3fv(uniID, 1, glm::value_ptr(LightDirection));
	
	uniID = glGetUniformLocation(my_program.program, "Shininess");
	glUniform1f(uniID, Shininess);

	uniID = glGetUniformLocation(my_program.program, "Strength");
	glUniform1f(uniID, Strength );
	
	uniID = glGetUniformLocation(my_program.program, "has_relief");
	glUniform1i(uniID, has_relief);
	
	uniID = glGetUniformLocation(my_program.program, "tex");
	glUniform1i(uniID, 0);

	
	uniID = glGetUniformLocation(my_program.program, "normalMap");
	glUniform1i(uniID, 1);


	uniID = glGetUniformLocation(my_program.program, "HeightMap");
	glUniform1i(uniID, 2);

	
	myModel.render();


	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_pos); //para testes com o cubo premade
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glDrawArrays(GL_PATCHES, 0, 12*3);
	
	
	glUseProgram(0);

	glutSwapBuffers();
	
}

void initGL(int w_size, int *argc, char **argv) {
	glutInit(argc, argv);
	glutInitWindowSize(w_size, w_size);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA  | GLUT_DEPTH);
	glutCreateWindow("mate21");
	
	glClearColor(1.0, 1.0, 1.0, 1.0);

	int err = glewInit();

	if (GLEW_OK != err) {
		printf("Error: %s\n", glewGetErrorString(err));
	}

	//glPolygonMode(GL_FRONT, GL_LINE);


	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
		

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutIdleFunc(idlefunc);
	glutKeyboardFunc(keyboard);

	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(mousewheel);


}

int main(int argc, char **argv) {
	initGL(WDIM, &argc, argv);
	//make_cube();
	myModel.loadfile("reliefmodel/cubo_normals.obj");
	myModel.loadtex("reliefmodel/bricks2.jpg");
	myModel.loadNormalMap("test2/normal_1.jpg");
	myModel.loadHeigthDepthMap("test2/height_1.jpg");

	initshaders();
	
	glutMainLoop();

	return 0;

}



