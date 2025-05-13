// homing_missile.cpp
#include "homing_missile.h"
#include "enemy_game_object.h"
#include <algorithm>
#include <iostream>

namespace game {

    HomingMissile::HomingMissile(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture,
        const glm::vec3& direction)
        : Projectile(position, geom, shader, texture, direction) {

        // Initialize homing missile properties
        target_ = nullptr;
        turning_speed_ = 3.0f;  // Radians per second - how fast it can turn
        damage_ = 0.7f;         // Lower damage than standard projectile

        // Set smaller scale for missiles
        SetScale(0.4f);

        // Initialize retarget timer - look for new target every 0.5 seconds
        retarget_timer_.Start(0.5f);
    }

    void HomingMissile::Update(double delta_time) {
        // If the missile is not active or has expired, don't update
        if (!IsActive() || IsExpired()) {
            return;
        }

        // Get current position and direction
        glm::vec3 position = GetPosition();
        glm::vec3 direction = GetDirection();

        // Simple straight-line movement if target is invalid
        if (target_ == nullptr || !target_->IsActive() || target_->IsExploding()) {
            position += direction * GetSpeed() * static_cast<float>(delta_time);
            SetPosition(position);

            // IMPORTANT: Always update rotation to match direction even without target
            SetRotation(atan2(direction.y, direction.x));

            GameObject::Update(delta_time);

            // Check if we need to find a new target
            if (retarget_timer_.Finished()) {
                retarget_timer_.Start(0.5f);
            }
            return;
        }

        // Target is valid, so we can safely access it
        // Calculate vector to target
        glm::vec3 to_target = target_->GetPosition() - position;
        float distance_to_target = glm::length(to_target);

        // Only adjust direction if target is far enough
        if (distance_to_target > 0.5f) {
            to_target = glm::normalize(to_target);

            // Turning logic
            float current_angle = atan2(direction.y, direction.x);
            float target_angle = atan2(to_target.y, to_target.x);

            // Find shortest angle
            float angle_diff = target_angle - current_angle;
            while (angle_diff > glm::pi<float>()) angle_diff -= 2.0f * glm::pi<float>();
            while (angle_diff < -glm::pi<float>()) angle_diff += 2.0f * glm::pi<float>();

            // Calculate turn amount with proper bounds
            float max_turn = turning_speed_ * static_cast<float>(delta_time);
            float turn_amount = (angle_diff > 0) ?
                std::min(max_turn, angle_diff) :
                std::max(-max_turn, angle_diff);

            // Apply turn
            float new_angle = current_angle + turn_amount;
            direction = glm::vec3(cos(new_angle), sin(new_angle), 0.0f);

            SetDirection(direction);
            SetRotation(new_angle);
        }

        // Update position
        position += direction * GetSpeed() * static_cast<float>(delta_time);
        SetPosition(position);

        // Check if we need to find a new target
        if (retarget_timer_.Finished()) {
            retarget_timer_.Start(0.5f);
        }

        // Parent update
        GameObject::Update(delta_time);
    }

} // namespace game