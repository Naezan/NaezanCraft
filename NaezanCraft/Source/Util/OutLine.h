#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class OutLine {
    int shaderProgram;
    unsigned int VBO, VAO, EBO, indicesSize;
    glm::vec3 position;
    glm::mat4 pv;
    glm::vec3 lineColor;
public:
    OutLine(const glm::vec3& _position);
    ~OutLine();

    void SetPV(glm::mat4 _pv) {
        pv = _pv;
    }

    void SetColor(glm::vec3 color) {
        lineColor = color;
    }

    void Render();

};