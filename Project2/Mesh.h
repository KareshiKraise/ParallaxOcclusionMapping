#pragma once
#include <GL\glew.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <vector>
#include <GLM/glm.hpp>

class Mesh
{
public:
	struct MeshEntry 
	{
		static enum BUFFERS {VERTEX_BUFFER = 0x00, TEX_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, TANGENT_BUFFER};
		GLuint vao;
		GLuint vbo[5];
						
		unsigned int elementCount;

		MeshEntry(aiMesh *mesh);
		~MeshEntry();

		void render();
	};

	std::vector<MeshEntry*> meshEntries;


	//std::vector<unsigned char> texData;

	

	//unsigned long width;
	//unsigned long height;
	
	GLuint texID;
	GLuint normalMapID;
	GLuint heightMapID;

	
		
	

	void loadtex(const char* path);
	void loadNormalMap(const char* path);
	void loadHeigthDepthMap(const char* path);
	
public:
	Mesh();

	void loadfile(const char* path);
	
	~Mesh();
	void render();
};

