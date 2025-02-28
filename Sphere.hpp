#ifndef SHPERE_HPP
#define SPHERE_HPP

#include <stdlib.h>
#include <stdio.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AGL3Window.hpp"
#include "AGL3Drawable.hpp"
#include "utils.hpp"



struct SphereDetails{
    float X,Y,Z;
    float radius;
};


const int SECTORS = 360;
const int STACKS = 180;
const float RADIUS = 1.0f;


class Sphere : public AGLDrawable {
public:
    Sphere(GLuint programId = 0) 
        : AGLDrawable(programId), position(0.0f), rotation(0.0f) ,color(1.0f, 1.0f, 1.0f) {
        setScale(glm::vec3(1.0));
        setShaders();
        initializeData();
    }

    virtual void setShaders() {
        compileShadersFromFile("Shaders/SphereNormal/SphereNormal.vert","Shaders/SphereNormal/SphereNormal.frag");
        compileShadersFromFile("Shaders/SphereNoise/SphereNoise.vert","Shaders/SphereNoise/SphereNoise.frag");
    }

    void initializeData() {
        bindVAO();

        vertices.clear();
        indices.clear();
        normals.clear();

        float x, y, z;

        float sector_step = 2 * M_PI / (float)SECTORS;
        float stack_step = M_PI / (float)STACKS;
        float invRadius = 1.0f/RADIUS;

        float sector_angle, stack_angle;

        for (int i = 0; i <= STACKS; ++i) {
            stack_angle = M_PI_2 - i * stack_step;
            z = RADIUS * sinf(stack_angle);

            for (int j = 0; j <= SECTORS; ++j) {
                sector_angle = j * sector_step;

                x = RADIUS * cosf(stack_angle) * cosf(sector_angle);
                y = RADIUS * cosf(stack_angle) * sinf(sector_angle);

                // Vertex position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normal (normalized vertex position)
                normals.push_back(x * invRadius);
                normals.push_back(y * invRadius);
                normals.push_back(z * invRadius);
            }
        }

        /*-------------------------Generowanie indeksów-----------------------*/

        int i_up;
        int i_down;

        for (int i = 0; i < STACKS; ++i) {
            i_up = i * (SECTORS + 1);
            i_down = i_up + SECTORS + 1;

            for (int j = 0; j < SECTORS; ++j, ++i_up, ++i_down) {
                if (i != 0) {
                    indices.push_back(i_up);
                    indices.push_back(i_down);
                    indices.push_back(i_up + 1);
                }

                if (i != (STACKS)) {
                    indices.push_back(i_up + 1);
                    indices.push_back(i_down);
                    indices.push_back(i_down + 1);
                }
            }
        }

        bindBuffers();
        glBufferData(GL_ARRAY_BUFFER, (unsigned int)(vertices.size() + normals.size()) * sizeof(float), nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, (unsigned int)vertices.size() * sizeof(float), vertices.data());
        glBufferSubData(GL_ARRAY_BUFFER, (unsigned int)vertices.size() * sizeof(float), (unsigned int)normals.size() * sizeof(float), normals.data());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(vertices.size() * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    SphereDetails getSphereDetails() const {
        return SphereDetails{
            position.x, position.y, position.z,   
            scale.x
        };
    }

    // Set the position of the Cube
    void setPosition(const glm::vec3& pos) {
        position = pos;
    }

    // Set the scale of the Cube
    void setScale(const glm::vec3& scl) {
        scale = scl;
    }

    // Set the rotation of the Cube (in radians)
    void setRotation(float angle) {
        rotation = angle;
    }

    // Get the current world position
    glm::vec3 getPosition() const {
        return position;
    }

    // Get the model matrix
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::rotate(model, rotation, glm::vec3(1.0f, 1.0f, 0.0f)); 
        model = glm::scale(model, scale);
        return model;
    }

    void setColor(const glm::vec3& newColor) {
        color = newColor;
    }
    virtual void PreDrawLogic(){
        return;
    }

    void draw(const glm::mat4& viewProjectionMatrix ) {

        int prog = 0;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        bindProgram(prog);
        bindVAO();
        PreDrawLogic();
        // Calculate new MVP matrix
        glm::mat4 mvp = viewProjectionMatrix * getModelMatrix();

        // Pass it to the shader
        GLuint modelLoc = glGetUniformLocation(p(prog), "mvp");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &mvp[0][0]);

        GLuint colorLoc = glGetUniformLocation(p(prog), "sphereColor");
        glUniform3fv(colorLoc, 1, &color[0]);

        //GLuint timeLoc = glGetUniformLocation(p(prog), "u_time");
        //glUniform1f(timeLoc, glfwGetTime());

        // Draw the cube
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

protected:
    glm::vec3 position; // Position in world space
    glm::vec3 scale;    // Scaling 
    float rotation;     // Rotation 
    glm::vec3 color;
    std::vector<float> vertices;
    std::vector<float> normals; // New normals vector
    std::vector<int> indices;
};




#endif