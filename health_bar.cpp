#include "health_bar.h"

namespace game {
    // Constructor
    HealthBar::HealthBar() : geometry_(nullptr), shader_(nullptr), background_(0), fill_(0) {
        position_ = glm::vec3(0.0f, 0.0f, 0.0f);
        width_ = 1.0f;
        height_ = 0.2f;
    }

    // Set up all the values in the beginning
    void HealthBar::Init(Geometry* geometry, Shader* shader, GLuint background, GLuint fill) {
        geometry_ = geometry;
        shader_ = shader;
        background_ = background;
        fill_ = fill;
    }

    void HealthBar::Render(const glm::mat4& view_matrix, float health) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Validate input and initialization
        if (!geometry_ || !shader_) {
            std::cerr << "ERROR: Health bar missing geometry or shader" << std::endl;
            return;
        }

        if (background_ == 0 || fill_ == 0) {
            std::cerr << "ERROR: Health bar missing textures (bg=" << background_
                << ", fill=" << fill_ << ")" << std::endl;
            return;
        }

        // Reset OpenGL state
        // glDisable(GL_DEPTH_TEST);
        health = glm::clamp(health, 0.0f, 1.0f);

        // Ensure shader is ready
        shader_->Enable();
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // USE SIMPLER APPROACH: Just draw two rectangles

        // 1. Draw background (black/gray bar)
        {
            //create scaling matrix
            glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(width_, height_, 1.0f));
            //create translation matrix
            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);
            //create transformation_matrix (T x S)
            glm::mat4 transformation_matrix = translation_matrix * scaling_matrix;
            
            //send transoformation matrix to shader
            shader_->SetUniformMat4("transformation_matrix", transformation_matrix);
            shader_->SetUniform1i("ghost_mode", 0); //don't need ghost mode
            geometry_->SetGeometry(shader_->GetShaderProgram());
            //draw backgound
            glBindTexture(GL_TEXTURE_2D, background_);
            glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
        }

        // 2. Draw health fill (colored bar) - simplified positioning
        if (health > 0.01f) {  // Ensure we have some health
            //width change based on the rest health
            glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(width_ * health, height_, 1.0f));
            glm::vec3 fill_position = position_;
            fill_position.x -=(width_ * (1.0f - health))/2.0f;
            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), fill_position);
            glm::mat4 transformation_matrix = translation_matrix * scaling_matrix;
            //send transoformation matrix to shader
            shader_->SetUniformMat4("transformation_matrix", transformation_matrix);
            shader_->SetUniform1i("ghost_mode", 0); //don't need ghost mode
            geometry_->SetGeometry(shader_->GetShaderProgram());
            glBindTexture(GL_TEXTURE_2D, fill_);
            glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
        }

        // Clean up
        shader_->Disable();
        glEnable(GL_DEPTH_TEST);
    }
}