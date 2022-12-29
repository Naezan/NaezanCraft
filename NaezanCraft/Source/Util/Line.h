#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Line {
    int shaderProgram;
    unsigned int VBO, VAO;
    std::vector<float> vertices;
    glm::vec3 startPoint;
    glm::vec3 endPoint;
    glm::mat4 MVP;
    glm::vec3 lineColor;
public:
    Line(glm::vec3 start, glm::vec3 end);
    ~Line();

    void setMVP(glm::mat4 mvp) {
        MVP = mvp;
    }

    void setColor(glm::vec3 color) {
        lineColor = color;
    }

    void Render();

};