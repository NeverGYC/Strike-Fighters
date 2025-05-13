#include "mini_map.h"
#include "game_object.h"
#include "player_game_object.h"
#include "enemy_game_object.h"
#include "boss_game_object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>

namespace game {

    MiniMap::MiniMap() {
        // Default position (top-left corner)
        pos_x_ = 30.0f;
        pos_y_ = 30.0f;

        // Default size
        width_ = 300.0f;
        height_ = 300.0f;

        // Default world bounds
        world_bound_x_ = 40.0f;
        world_bound_y_ = 40.0f;

        // Much brighter colors with higher contrast
        player_color_ = glm::vec3(0.0f, 1.0f, 1.0f);     // Cyan for player
        enemy_color_ = glm::vec3(1.0f, 0.0f, 0.0f);      // Bright red for enemies
        boss_color_ = glm::vec3(0.0f, 1.0f, 0.0f);       // Green for boss
        background_color_ = glm::vec3(0.0f, 0.0f, 0.1f);

        // Initialize pointers
        geometry_ = nullptr;
        shader_ = nullptr;
    }

    void MiniMap::Init(Geometry* geom, Shader* shader) {
        if (!geom || !shader) {
            std::cerr << "ERROR: Invalid geometry or shader for MiniMap" << std::endl;
            return;
        }

        geometry_ = geom;
        shader_ = shader;
    }


    void MiniMap::Render(const std::vector<GameObject*>& game_objects, BossGameObject* boss) {
        // Make sure we have valid resources
        if (!geometry_ || !shader_ || shader_->GetShaderProgram() == 0) {
            std::cerr << "ERROR: MiniMap not properly initialized" << std::endl;
            return;
        }

        // For debugging
        static int frame_count = 0;
        frame_count++;

        // Save OpenGL state
        GLboolean depth_test_enabled;
        glGetBooleanv(GL_DEPTH_TEST, &depth_test_enabled);

        // Set up rendering
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Get window size
        int window_width, window_height;
        glfwGetWindowSize(glfwGetCurrentContext(), &window_width, &window_height);

        // Setup simple orthographic projection
        glm::mat4 projection = glm::ortho(
            0.0f, (float)window_width,
            (float)window_height, 0.0f,
            -1.0f, 1.0f
        );

        // Enable shader
        shader_->Enable();
        shader_->SetUniformMat4("view_matrix", projection);

        // Draw a simple background rectangle
        // Make background semi-transparent - USE CLASS VARIABLE
        shader_->SetUniform3f("color_override", background_color_);
        shader_->SetUniform1i("use_color_override", 1);

        glm::mat4 bg_transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos_x_ + width_ / 2, pos_y_ + height_ / 2, 0.0f));
        bg_transform = glm::scale(bg_transform, glm::vec3(width_, height_, 1.0f));
        shader_->SetUniformMat4("transformation_matrix", bg_transform);

        // Draw background
        geometry_->SetGeometry(shader_->GetShaderProgram());
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

        // Draw border
        glm::vec3 border_color(1.0f, 1.0f, 1.0f);
        shader_->SetUniform3f("color_override", border_color);

        // Very simple border (just a slightly larger rectangle)
        float border_size = 3.0f;
        glm::mat4 border_transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos_x_ + width_ / 2, pos_y_ + height_ / 2, 0.0f));
        border_transform = glm::scale(border_transform, glm::vec3(width_ + border_size * 2, height_ + border_size * 2, 1.0f));
        shader_->SetUniformMat4("transformation_matrix", border_transform);

        // Draw border
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

        // Now draw dots for game objects
        // Get player first (always first object)
        PlayerGameObject* player = nullptr;
        if (!game_objects.empty()) {
            player = dynamic_cast<PlayerGameObject*>(game_objects[0]);
        }

        // If we have a player, draw all dots with positions relative to player
        if (player) {
            // Draw player dot - USE CLASS VARIABLE
            glm::vec3 player_pos = player->GetPosition();
            DrawMapDot(player_pos, player_color_, 8.0f);

            // Draw enemy dots - USE CLASS VARIABLE
            for (const auto& obj : game_objects) {
                if (obj != player && dynamic_cast<EnemyGameObject*>(obj) && obj->IsActive()) {
                    DrawMapDot(obj->GetPosition(), enemy_color_, 5.0f);
                }
            }

            // Draw boss - USE CLASS VARIABLE instead of local variable
            if (boss && boss->IsActive()) {
                DrawMapDot(boss->GetPosition(), boss_color_, 10.0f);
            }
        }

        // Done, restore states
        shader_->SetUniform1i("use_color_override", 0);
        shader_->Disable();

        if (depth_test_enabled) {
            glEnable(GL_DEPTH_TEST);
        }
    }



    void MiniMap::DrawMapDot(const glm::vec3& world_pos, const glm::vec3& color, float size) {
        // Skip dots outside world bounds (with some margin)
        if (fabs(world_pos.x) > world_bound_x_ * 1.1f || fabs(world_pos.y) > world_bound_y_ * 1.1f) {
            return;
        }

        // Calculate normalized position within minimap (0-1 range)
        float norm_x = (world_pos.x + world_bound_x_) / (2.0f * world_bound_x_);
        float norm_y = 1.0f - ((world_pos.y + world_bound_y_) / (2.0f * world_bound_y_));

        // Calculate position in screen space
        float screen_x = pos_x_ + (norm_x * width_);
        float screen_y = pos_y_ + (norm_y * height_);

        // Set color for dot
        shader_->SetUniform3f("color_override", color);

        // Create dot transformation
        glm::mat4 dot_transform = glm::translate(glm::mat4(1.0f), glm::vec3(screen_x, screen_y, 0.0f));
        dot_transform = glm::scale(dot_transform, glm::vec3(size, size, 1.0f));
        shader_->SetUniformMat4("transformation_matrix", dot_transform);

        // Draw dot
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
    }

} // namespace game