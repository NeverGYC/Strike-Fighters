// fast_enemy_game_object.cpp
#include "fast_enemy_game_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

    FastEnemyGameObject::FastEnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : EnemyGameObject(position, geom, shader, texture) {

        // Set type identifier
        SetEnemyType(1);

        // Configure fast enemy specific attributes
        speed_ = 2.0f;              // 75% faster than base enemy
        detection_range_ = 7.0f;    // Longer detection range
        SetHealth(0.7f);            // Lower health
        SetDamage(0.5f);            // Lower damage

        // Set attack parameters
        attack_speed_multiplier_ = 1.8f;  // Fast attack speed

        // Initialize strafing behavior
        strafe_timer_.Start(1.0f);
        strafe_direction_ = true;  // Start by strafing right
    }

    void FastEnemyGameObject::Update(double delta_time) {
        // Handle strafing behavior timer
        if (strafe_timer_.Finished()) {
            // Switch strafe direction
            strafe_direction_ = !strafe_direction_;
            // Randomize next strafe time between 0.8 and 1.2 seconds
            float next_strafe = 0.8f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.4f;
            strafe_timer_.Start(next_strafe);
        }

        // Call parent class update method
        EnemyGameObject::Update(delta_time);
    }

    // In fast_enemy_game_object.cpp
    bool FastEnemyGameObject::FireProjectile() {
        // Fast enemies fire more frequently
        if (!IsActive() || IsExploding() || !attack_cooldown_timer_.Finished()) {
            return false;
        }

        // Reset cooldown timer - shorter for fast enemies
        attack_cooldown_timer_.Start(0.8f); // Fire more frequently

        return true;
    }



    void FastEnemyGameObject::UpdateAttackMovement(double delta_time) {
        // Get current position
        glm::vec3 position = GetPosition();

        // Calculate direction to target
        glm::vec3 direction = target_position_ - position;
        float distance_to_target = glm::length(direction);

        // Always face the player when in attack mode
        if (distance_to_target > 0.001f) {
            direction = glm::normalize(direction);
            // Set rotation to face player directly
            SetRotation(atan2(direction.y, direction.x));
        }

        // Define optimal combat distance
        float optimal_distance = GetIdealDistance();

        // Calculate approach/retreat factor based on current distance
        float distance_factor = CalculateDistanceFactor(distance_to_target, optimal_distance);

        // Get perpendicular vector for strafing
        glm::vec3 perpendicular = glm::vec3(-direction.y, direction.x, 0.0f);

        // Calculate acceleration instead of directly setting velocity
        float approach_speed = speed_ * distance_factor * 0.8f;
        float strafe_speed = speed_ * 1.2f;

        // Reset acceleration
        acceleration_ = glm::vec3(0.0f);

        if (strafe_direction_) {
            // Strafe right with approach/retreat
            acceleration_ = direction * approach_speed + perpendicular * strafe_speed;
        }
        else {
            // Strafe left with approach/retreat
            acceleration_ = direction * approach_speed - perpendicular * strafe_speed;
        }

        // Scale acceleration to be an actual acceleration, not a velocity
        acceleration_ *= 2.0f;

        // Update velocity with physics
        velocity_ += acceleration_ * static_cast<float>(delta_time);

        // Apply maximum speed limit
        float max_speed = speed_ * attack_speed_multiplier_;
        float current_speed = glm::length(velocity_);
        if (current_speed > max_speed) {
            velocity_ = velocity_ * (max_speed / current_speed);
        }

        // Apply smaller damping for smoother movement
        velocity_ *= 0.98f;

        // Update position based on velocity
        position += velocity_ * static_cast<float>(delta_time);
        SetPosition(position);

        // Check if we can attack - only fire within reasonable range
        if (CanAttack() && distance_to_target < optimal_distance * 1.5f) {
            if (FireProjectile()) {
                std::cout << "Fast enemy firing!" << std::endl;
            }
        }
    }
} // namespace game