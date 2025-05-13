#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include "geometry.h"
#include "shader.h"

#include "game_object.h"
#include "player_game_object.h"

namespace game {

    GameObject::GameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
    {
        if (!geom || !shader) {
            throw std::runtime_error("Invalid geometry or shader in GameObject constructor");
        }

        // Initialize all attributes
        position_ = position;
        scale_ = glm::vec2(1.0f, 1.0f);  // Default uniform scaling
        angle_ = 0.0;
        geometry_ = geom;
        shader_ = shader;
        texture_ = texture;
        original_texture_ = texture;
        explosion_texture_ = texture;
        is_active_ = true;
        is_exploding_ = false;
        is_ghost_ = false;  // Default: not in ghost mode
    }


    glm::vec3 GameObject::GetBearing(void) const {
        glm::vec3 dir(cos(angle_), sin(angle_), 0.0);
        return dir;
    }


    glm::vec3 GameObject::GetRight(void) const {
        float pi_over_two = glm::pi<float>() / 2.0f;
        glm::vec3 dir(cos(angle_ - pi_over_two), sin(angle_ - pi_over_two), 0.0);
        return dir;
    }


    void GameObject::SetRotation(float angle) {
        // Set rotation angle of the game object
        // Make sure angle is in the range [0, 2*pi]
        float two_pi = 2.0f * glm::pi<float>();
        angle = fmod(angle, two_pi);
        if (angle < 0.0) {
            angle += two_pi;
        }
        angle_ = angle;
    }


    void GameObject::Update(double delta_time) {
        if (is_exploding_ && explosion_timer_.Finished()) {
            is_active_ = false;
            // Don't reset the texture for player explosion
            // Only reset for regular game objects (enemies)
            if (dynamic_cast<PlayerGameObject*>(this) == nullptr) {
                texture_ = original_texture_;
            }
        }
    }

    void GameObject::StartExplosion() {
        is_exploding_ = true;
        texture_ = explosion_texture_;
        explosion_timer_.Start(5.0f);
    }


    void GameObject::Render(glm::mat4 view_matrix, double current_time) {
        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Setup the scaling matrix for the shader using the 2D scale vector
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_.x, scale_.y, 1.0f));

        // Setup the rotation matrix for the shader
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

        // Set up the translation matrix for the shader
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

        // Setup the transformation matrix for the shader
        glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

        // Set the transformation matrix in the shader
        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

        // Set ghost mode uniform in shader (convert bool to int for GLSL)
        shader_->SetUniform1i("ghost_mode", is_ghost_ ? 1 : 0);

        // Set up the geometry
        geometry_->SetGeometry(shader_->GetShaderProgram());

        // Bind the entity's texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Draw the entity
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
    }

} // namespace game