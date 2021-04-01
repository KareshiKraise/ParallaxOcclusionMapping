#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Mesh.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <iostream>
#include "picoPNG.h"
#include <SOIL/SOIL.h>


Mesh::MeshEntry::MeshEntry(aiMesh *mesh)
{
	vbo[VERTEX_BUFFER] = NULL;
	vbo[TEX_BUFFER] = NULL;
	vbo[NORMAL_BUFFER] = NULL;
	vbo[INDEX_BUFFER] = NULL;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	elementCount = mesh->mNumFaces * 3;
	if (mesh->HasPositions())
	{
		float *vertices = new float[mesh->mNumVertices * 3];
		for (size_t i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}
		glGenBuffers(1, &vbo[VERTEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		delete[] vertices;

	}
	
	if (mesh->HasTextureCoords(0))
	{
		float *texCoords = new float[mesh->mNumVertices * 2];
		for (size_t i = 0; i < mesh->mNumVertices; ++i)
		{
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &vbo[TEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[TEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		delete[] texCoords;
				


	}

	if (mesh->HasNormals())
	{
		float *normals = new float[mesh->mNumVertices * 3];
		for (size_t i = 0; i < mesh->mNumVertices; ++i)
		{
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &vbo[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(2,3 , GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		delete[] normals;
		
	}

	if (mesh->HasFaces())
	{
		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
		for (size_t i = 0; i < mesh->mNumFaces; ++i)
		{
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &vbo[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,  3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW );

		glVertexAttribPointer(3, 3 , GL_UNSIGNED_INT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);
		
		delete[] indices;
	}

	
	//tangents
	float *tangents = new float[mesh->mNumVertices * 3];
	//calculo manual das tangentes
	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		tangents[i * 3] = mesh->mTangents[i].x;
		tangents[i * 3 + 1] = mesh->mTangents[i].y;
		tangents[i * 3 + 2] = mesh->mTangents[i].z;
	}

	glGenBuffers(1, &vbo[TANGENT_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TANGENT_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(float), tangents, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);
	delete[] tangents;

	
	
	


	glBindBuffer(GL_ARRAY_BUFFER ,0);
	glBindVertexArray(0);

}



Mesh::MeshEntry::~MeshEntry()
{
	if (vbo[VERTEX_BUFFER])
	{
		glDeleteBuffers(1, &vbo[VERTEX_BUFFER]);
	}

	if (vbo[TEX_BUFFER])
	{
		glDeleteBuffers(1, &vbo[TEX_BUFFER]);
	}

	if (vbo[NORMAL_BUFFER])
	{
		glDeleteBuffers(1, &vbo[NORMAL_BUFFER]);
	}

	if (vbo[INDEX_BUFFER])
	{
		glDeleteBuffers(1, &vbo[INDEX_BUFFER]);
	}
}

void Mesh::MeshEntry::render()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[TANGENT_BUFFER]);
	glBindVertexArray(vao);
	
	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

Mesh::Mesh()
{
	//
}

void Mesh::loadfile(const char* path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_CalcTangentSpace);
	if (!scene)
	{
		printf("Unable to load mesh %s \n", importer.GetErrorString());
	}

	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		meshEntries.push_back(new Mesh::MeshEntry(scene->mMeshes[i]));
	}
}

void Mesh::loadtex(const char* path)
{
	
	texID = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, 0, SOIL_FLAG_MIPMAPS);
}

/*
void Mesh::setGLTexture()
{
	//glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, (GLsizei)width, (GLsizei)height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)width, (GLsizei)height, GL_RGBA, GL_UNSIGNED_BYTE, &texData[0]);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}*/

void Mesh::loadNormalMap(const char* path)
{
	normalMapID = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, 0, SOIL_FLAG_MIPMAPS);
	
	
}

void Mesh::loadHeigthDepthMap(const char* path)
{
	heightMapID = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, 0, SOIL_FLAG_MIPMAPS);
}


Mesh::~Mesh()
{
	for (size_t i = 0; i < meshEntries.size(); ++i)
	{
		delete meshEntries.at(i);
	}
	meshEntries.clear();
}

void Mesh::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapID);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMapID);


	for (size_t i = 0; i < meshEntries.size(); ++i )
	{
		
		meshEntries.at(i)->render();
	}
}