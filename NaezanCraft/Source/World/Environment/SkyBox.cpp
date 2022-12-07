#include "../../pch.h"
#include "SkyBox.h"
#include "../Mesh.h"
#include "../../Renderer/Environment/SkyBoxShader.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Buffer.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SkyBox::SkyBox()
{
	skyBoxShaders.emplace(ShaderType::VERTEX, std::make_unique<SkyBoxShader>("../Assets/Shaders/ProcSky.vs", ShaderType::VERTEX));
	skyBoxShaders.emplace(ShaderType::FRAGMENT, std::make_unique<SkyBoxShader>("../Assets/Shaders/ProcSky.fs", ShaderType::FRAGMENT));

    constexpr GLfloat SIZE = 500;

    std::vector<GLfloat> vertexCoords{
        //Back
        SIZE, -SIZE, -SIZE, 3,
        -SIZE, -SIZE, -SIZE, 0,
        -SIZE,  SIZE, -SIZE, 1,
        SIZE,  SIZE, -SIZE, 2,

        //Front
        -SIZE, -SIZE, SIZE, 4,
        SIZE, -SIZE, SIZE, 7,
        SIZE,  SIZE, SIZE, 5,
        -SIZE,  SIZE, SIZE, 6,

        //Right
        SIZE, -SIZE,  SIZE, 7,
        SIZE, -SIZE, -SIZE, 3,
        SIZE,  SIZE, -SIZE, 2,
        SIZE,  SIZE,  SIZE, 5,

        //Left
        -SIZE, -SIZE, -SIZE, 0,
        -SIZE, -SIZE,  SIZE, 4,
        -SIZE,  SIZE,  SIZE, 6,
        -SIZE,  SIZE, -SIZE, 1,

        //Top
        -SIZE, SIZE,  SIZE, 6,
        SIZE, SIZE,  SIZE, 5,
        SIZE, SIZE, -SIZE, 2,
        -SIZE, SIZE, -SIZE, 1,

        //Bottom
        -SIZE, -SIZE, -SIZE, 0,
        SIZE, -SIZE, -SIZE, 3,
        SIZE, -SIZE,  SIZE, 7,
        -SIZE, -SIZE,  SIZE, 4,
    };

    std::vector<GLuint> indices{
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    vertexArray = VertexArray::CreateArray();
    vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(0, (void*)0);
    vertexBuffer->SetBufferData(vertexCoords.size() * sizeof(GLfloat), vertexCoords.data());
    indexBuffer = Buffer::CreateBuffer<IndexBuffer>(indices.size() * sizeof(GLuint), indices.data());
    indicesSize = indices.size();
    indices.clear();
}

SkyBox::~SkyBox() = default;

void SkyBox::Render(std::shared_ptr<Camera>& camera)
{
    for (auto& shader : skyBoxShaders)
    {
        shader.second->Use();
    }

	skyBoxShaders[ShaderType::VERTEX]->Update(camera);


    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr);
}
