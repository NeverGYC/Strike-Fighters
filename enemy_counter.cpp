#include "enemy_counter.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace game {

    // Constructor
    EnemyCounter::EnemyCounter(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : TextGameObject(position, geom, shader, texture) {
        enemy_count_ = 0;
        UpdateDisplayText();
    }

    // Increment the enemy counter
    void EnemyCounter::IncrementCount() {
        enemy_count_++;
        UpdateDisplayText();
    }

    // Reset the counter
    void EnemyCounter::ResetCount() {
        enemy_count_ = 0;
        UpdateDisplayText();
    }

    // Get the current count
    int EnemyCounter::GetCount() const {
        return enemy_count_;
    }

    // Helper to format the counter display
    void EnemyCounter::UpdateDisplayText() {
        std::stringstream ss;
        ss << "Enemies Destroyed: " << enemy_count_;
        count_text_ = ss.str();
        SetText(count_text_);
    }

    // Update method
    void EnemyCounter::Update(double delta_time) {
        TextGameObject::Update(delta_time);
    }

    // Render UI method
    void EnemyCounter::RenderUI(glm::mat4 ui_projection_matrix, double current_time) {
        // Enable blending for smooth text rendering
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Safety checks
        if (!geometry_ || !shader_) {
            std::cerr << "Error: Geometry or shader not initialized for EnemyCounter" << std::endl;
            return;
        }

        // Get text to render
        std::string text = GetText();
        if (text.empty()) {
            return;
        }

        // Ensure shader is enabled and configured
        shader_->Enable();

        // Set up the UI projection matrix
        shader_->SetUniformMat4("view_matrix", ui_projection_matrix);

        // Setup scaling matrix (adjust scale to ensure visibility)
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f),
            glm::vec3(xscale_ * 2.0f, yscale_ * 2.0f, 1.0f));

        // Setup translation matrix based on current position
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

        // Combine transformations
        glm::mat4 transformation_matrix = translation_matrix * scaling_matrix;

        // Set transformation matrix
        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);
        shader_->SetUniform1i("ghost_mode", 0);

        // Prepare geometry
        geometry_->SetGeometry(shader_->GetShaderProgram());

        // Bind font texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Prepare text rendering parameters
#define TEXT_LENGTH 40
        int final_size = text.size();
        if (final_size > TEXT_LENGTH) {
            final_size = TEXT_LENGTH;
        }
        shader_->SetUniform1i("text_len", final_size);

        // Convert text to integer array for shader
        GLint text_data[TEXT_LENGTH];
        for (int i = 0; i < final_size; i++) {
            text_data[i] = text[i];
        }
        shader_->SetUniformIntArray("text_content", final_size, text_data);

        // Draw text
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

        // Clean up
        shader_->Disable();
        glDisable(GL_BLEND);
    }

} // namespace game