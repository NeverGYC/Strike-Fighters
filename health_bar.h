#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include "shader.h"
#include "geometry.h"

namespace game {
    class HealthBar {
    public:
        // Constructor
        HealthBar();

        bool IsInitialized() const {
            return geometry_ != nullptr && shader_ != nullptr &&
                background_ != 0 && fill_ != 0;
        }

        // Initialize the health bar
        void Init(Geometry* geometry, Shader* shader, GLuint background, GLuint fill);

        // Setter
        void SetPosition(const glm::vec3& position) { position_ = position; }
        void SetSize(float width, float height) { width_ = width; height_ = height; }

        // Render function
        void Render(const glm::mat4& view_matrix, float health);

    private:
        GLuint background_;  // Background texture
        GLuint fill_;        // Fill texture (the part that shows health)

        Geometry* geometry_; // Geometry to render the bar
        Shader* shader_;     // Shader to use

        glm::vec3 position_; // Position on screen
        float width_;        // Width of the bar
        float height_;       // Height of the bar
    };
}

#endif // HEALTH_BAR_H