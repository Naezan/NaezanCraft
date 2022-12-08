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
    //Shader
	skyBoxShaders.emplace(ShaderType::VERTEX, std::make_unique<SkyBoxShader>("../Assets/Shaders/ProcSky.vs", ShaderType::VERTEX));
	skyBoxShaders.emplace(ShaderType::FRAGMENT, std::make_unique<SkyBoxShader>("../Assets/Shaders/ProcSky.fs", ShaderType::FRAGMENT));

    shaderProgram = glCreateProgram();

    for (auto& shader : skyBoxShaders)
    {
        glAttachShader(shaderProgram, shader.second->GetShaderID());
    }

    glLinkProgram(shaderProgram);

    for (auto& shader : skyBoxShaders)
    {
        shader.second->LinkComplete(shaderProgram);
    }

    skyBoxShaders[ShaderType::VERTEX]->GetUniform(shaderProgram);

    //Mesh
    //skyMesh = std::make_unique<Mesh>();

    //4th value is weight value, for change specific weather color
    std::vector<GLfloat> vertexCoords{
        //Back
        SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 3,
        -SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 0,
        -SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 1,
        SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 2,

        //Front
        -SKY_SIZE, -SKY_SIZE, SKY_SIZE, 4,
        SKY_SIZE, -SKY_SIZE, SKY_SIZE, 7,
        SKY_SIZE,  SKY_SIZE, SKY_SIZE, 5,
        -SKY_SIZE,  SKY_SIZE, SKY_SIZE, 6,

        //Right
        SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 7,
        SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 3,
        SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 2,
        SKY_SIZE,  SKY_SIZE,  SKY_SIZE, 5,

        //Left
        -SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 0,
        -SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 4,
        -SKY_SIZE,  SKY_SIZE,  SKY_SIZE, 6,
        -SKY_SIZE,  SKY_SIZE, -SKY_SIZE, 1,

        //Top
        -SKY_SIZE, SKY_SIZE,  SKY_SIZE, 6,
        SKY_SIZE, SKY_SIZE,  SKY_SIZE, 5,
        SKY_SIZE, SKY_SIZE, -SKY_SIZE, 2,
        -SKY_SIZE, SKY_SIZE, -SKY_SIZE, 1,

        //Bottom
        -SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 0,
        SKY_SIZE, -SKY_SIZE, -SKY_SIZE, 3,
        SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 7,
        -SKY_SIZE, -SKY_SIZE,  SKY_SIZE, 4,
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
    vertexArray->Bind();

    vertexBuffer = Buffer::CreateBuffer<VertexBuffer>(0, (void*)0);
    vertexBuffer->SetBufferData(vertexCoords.size() * sizeof(GLfloat), &vertexCoords.front());

    vertexArray->Bind();
    indexBuffer = Buffer::CreateBuffer<IndexBuffer>(indices.size() * sizeof(GLuint), &indices.front());
    indicesSize = indices.size();
    indices.clear();
}

SkyBox::~SkyBox() = default;

void SkyBox::Render(std::shared_ptr<Camera>& camera)
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glUseProgram(shaderProgram);

    vertexArray->Bind();
	skyBoxShaders[ShaderType::VERTEX]->Update(camera);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, nullptr);
    vertexArray->UnBind();
}
