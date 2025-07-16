#include "Mesh.h"

Mesh::Mesh()
{
}

void Mesh::loadFromAssimp(aiMesh* assimpMesh)
{
	for (int i = 0; i < assimpMesh->mNumVertices; ++i)
	{
		aiVector3D pos = assimpMesh->mVertices[i];
		vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
	}

	for (int i = 0; i < assimpMesh->mNumFaces; i++)
	{
		indices.push_back(assimpMesh->mFaces[i].mIndices[0]);
		indices.push_back(assimpMesh->mFaces[i].mIndices[1]);
		indices.push_back(assimpMesh->mFaces[i].mIndices[2]);
	}

	if (assimpMesh->HasTextureCoords(0))
	{
		for (int i = 0; i < assimpMesh->mNumVertices; ++i)
		{
			aiVector3D uv = assimpMesh->mTextureCoords[0][i];
			texCoords.push_back(glm::vec2(uv.x, -uv.y));
		}
	}

	if (assimpMesh->HasNormals())
	{
		for (int i = 0; i < assimpMesh->mNumVertices; ++i)
		{
			aiVector3D n = assimpMesh->mNormals[i];
			normals.push_back(glm::vec3(n.x, n.y, n.z));
		}
	}

	if (assimpMesh->HasTangentsAndBitangents())
	{
		for (int i = 0; i < assimpMesh->mNumVertices; ++i)
		{
			aiVector3D t = assimpMesh->mTangents[i];
			tangents.push_back(glm::vec3(t.x, t.y, t.z));
		}

		for (int i = 0; i < assimpMesh->mNumVertices; ++i)
		{
			aiVector3D b = assimpMesh->mBitangents[i];
			bitangents.push_back(glm::vec3(b.x, b.y, b.z));
		}
	}


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	if (texCoords.size() > 0)
	{
		glGenBuffers(1, &texCoordsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordsBuffer);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
	}

	if (normals.size() > 0)
	{
		glGenBuffers(1, &normalsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	}

	if (tangents.size() > 0)
	{
		glGenBuffers(1, &tangentsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	}

	if (bitangents.size() > 0)
	{
		glGenBuffers(1, &bitangentsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, bitangentsBuffer);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void Mesh::draw()
{
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsBuffer);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
	glVertexAttribPointer(
		3,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentsBuffer);
	glVertexAttribPointer(
		4,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

	glDrawElements(
		GL_TRIANGLES,
		indices.size(),
		GL_UNSIGNED_INT,
		(void*)0
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	glBindVertexArray(0);
}
