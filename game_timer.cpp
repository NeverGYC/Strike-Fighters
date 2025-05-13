#include "game_timer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace game {
    // Constructor
    GameTimer::GameTimer(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : TextGameObject(position, geom, shader, texture) {
        start_time_ = glfwGetTime();
        last_update_seconds_ = -1;
        last_time_string_ = "Time: 00:00";
        SetText(last_time_string_);
        SetScale(3.0f, 0.8f);
    }

    // Make the seconds to format "min:sec"
    std::string GameTimer::FormatTime(double seconds) const {
        int total_seconds = static_cast<int>(seconds);
        int min = total_seconds / 60;
        int sec = total_seconds % 60;

        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << min << ":";
        ss << std::setw(2) << std::setfill('0') << sec;
        return ss.str();
    }

    void GameTimer::Render(glm::mat4 view_matrix, double current_time) {
        TextGameObject::Render(view_matrix, current_time);
    }

    void GameTimer::RenderUI(glm::mat4 ui_projection_matrix, double current_time) {
        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Safety checks
        if (!geometry_ || !shader_) {
            std::cerr << "Error: Geometry or shader not initialized" << std::endl;
            return;
        }

        std::string text = GetText();
        if (text.empty()) {
            return;
        }

        shader_->Enable();
        shader_->SetUniformMat4("view_matrix", ui_projection_matrix);

        // Setup scaling and translation
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f),
            glm::vec3(xscale_ * 2.0f, yscale_ * 2.0f, 1.0f));
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);
        glm::mat4 transformation_matrix = translation_matrix * scaling_matrix;

        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);
        shader_->SetUniform1i("ghost_mode", 0);

        geometry_->SetGeometry(shader_->GetShaderProgram());
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Prepare text rendering
#define TEXT_LENGTH 40
        int final_size = text.size();
        if (final_size > TEXT_LENGTH) {
            final_size = TEXT_LENGTH;
        }
        shader_->SetUniform1i("text_len", final_size);

        GLint text_data[TEXT_LENGTH];
        for (int i = 0; i < final_size; i++) {
            text_data[i] = text[i];
        }
        shader_->SetUniformIntArray("text_content", final_size, text_data);

        // Draw text
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

        shader_->Disable();
        glDisable(GL_BLEND);
    }

    void GameTimer::Update(double delta_time) {
        double current_time = glfwGetTime(); // Get the current time
        // Calculate the duration
        double duration = current_time - start_time_;
        int current_seconds = static_cast<int>(duration);

        // Update the text
        if (current_seconds != last_update_seconds_) {
            last_time_string_ = "Time: " + FormatTime(duration);
            SetText(last_time_string_);
            last_update_seconds_ = current_seconds;
        }
        TextGameObject::Update(delta_time);
    }
}