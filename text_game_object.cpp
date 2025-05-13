#include "text_game_object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define TEXT_LENGTH 40

namespace game {
    TextGameObject::TextGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : GameObject(position, geom, shader, texture) {
        text_ = "";
    }

    std::string TextGameObject::GetText(void) const {
        return text_;
    }

    void TextGameObject::SetText(std::string text) {
        text_ = text;
    }

    void TextGameObject::Render(glm::mat4 view_matrix, double current_time) {
        // Early abort if no text (only check once)
        if (text_.empty()) {
            return;
        }

        // Add error checking for texture
        if (texture_ == 0) {
            std::cerr << "ERROR: Invalid font texture ID" << std::endl;
            return;
        }

        // Check shader program is valid
        if (shader_->GetShaderProgram() == 0) {
            std::cerr << "ERROR: Invalid shader program for text" << std::endl;
            return;
        }

        // Ensure text isn't too long
        if (text_.size() > TEXT_LENGTH) {
            std::cerr << "WARNING: Text too long, truncating: " << text_ << std::endl;
        }

        // Ensure proper blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Safety checks with detailed errors
        if (!geometry_) {
            std::cerr << "ERROR: TextGameObject - geometry is null" << std::endl;
            return;
        }
        if (!shader_) {
            std::cerr << "ERROR: TextGameObject - shader is null" << std::endl;
            return;
        }

        // Clear any pending OpenGL errors
        while (glGetError() != GL_NO_ERROR) {}

        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Setup the scaling matrix for the shader (increased scale)
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(xscale_ * 3.0f, yscale_ * 3.0f, 1.0f));

        // Setup the rotation matrix for the shader
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

        // Set up the translation matrix for the shader
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

        // Setup the transformation matrix for the shader
        glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

        // Set the transformation matrix in the shader
        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);
        shader_->SetUniform1i("ghost_mode", 0);

        // Set up the geometry
        geometry_->SetGeometry(shader_->GetShaderProgram());

        // Bind the entity's texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        int final_size = std::min((int)text_.size(), TEXT_LENGTH);
        shader_->SetUniform1i("text_len", final_size);

        // Set the text data - initialize all with zeros
        GLint data[TEXT_LENGTH] = { 0 };
        // Copy only what fits in our buffer
        for (int i = 0; i < final_size; i++) {
            data[i] = (GLint)text_[i]; // Cast to GLint explicitly
        }

        // IMPORTANT: Always pass TEXT_LENGTH as the count to match shader's uniform array size
        try {
            shader_->SetUniformIntArray("text_content", TEXT_LENGTH, data);
        }
        catch (const std::exception& e) {
            std::cerr << "Error setting text uniform array: " << e.what() << std::endl;
        }

        // Draw the entity
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

        shader_->Disable();
        glDisable(GL_BLEND);
    }
} // namespace game