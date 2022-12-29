#include "../pch.h"
#include "OutLine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

OutLine::OutLine(const glm::vec3& _position) : position(_position)
{
    lineColor = glm::vec3(0, 1, 0);
    pv = glm::mat4(1.0f);

    const char* vertexShaderSource = "#version 330\n"
        "layout(location = 0) in vec3 in_Pos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projectionview;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projectionview * model * vec4(in_Pos, 1.0);\n"
        "}\n\0";
    const char* fragmentShaderSource = "#version 330\n"
        "out vec4 Outcolor;\n"
        "uniform vec3 color;\n"
        "void main()\n"
        "{\n"
        "   Outcolor = vec4(color, 1.0f);\n"
        "}\n\0";

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors

    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<GLfloat> vertexCoords{
        -0.05f,  1.05f, -0.05f,
         1.05f,  1.05f, -0.05f,
         1.05f,  1.05f,  1.05f,
        -0.05f,  1.05f,  1.05f,

        -0.05f, -0.05f, -0.05f,
         1.05f, -0.05f, -0.05f,
         1.05f, -0.05f,  1.05f,
        -0.05f, -0.05f,  1.05f,
    };

    std::vector<GLuint> indexCoords{
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        4, 5,
        5, 6,
        6, 7,
        7, 4,

        0, 4,
        1, 5,
        2, 6,
        3, 7,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCoords.size() * sizeof(GLfloat), vertexCoords.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCoords.size() * sizeof(GLuint),
        indexCoords.data(), GL_STATIC_DRAW);
    indicesSize = indexCoords.size();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

OutLine::~OutLine()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void OutLine::Render()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionview"), 1, GL_FALSE, glm::value_ptr(pv));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

    glLineWidth(2);
    glDrawElements(GL_LINES, indicesSize, GL_UNSIGNED_INT, nullptr);
}
