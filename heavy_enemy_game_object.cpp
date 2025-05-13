// heavy_enemy_game_object.cpp
#include "heavy_enemy_game_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

    HeavyEnemyGameObject::HeavyEnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : EnemyGameObject(position, geom, shader, texture) {

        // Set type identifier
        SetEnemyType(2);

        // Configure heavy enemy specific attributes
        speed_ = 1.0f;              // 40% slower than base enemy
        detection_range_ = 10.0f;
        SetHealth(2.5f);            // Much higher health
        SetDamage(2.0f);            // Double damage

        // Set charging attack parameters
        charge_speed_ = 5.0f;       // Fast charge speed
        is_charging_ = false;

        // Initialize charge timers
        charge_timer_.Start(0.0f);     // Not charging initially
        charge_cooldown_.Start(5.0f);  // Initial cooldown

        // Create larger scale for heavy enemy
        SetScale(1.5f);
    }

    // Similarly for HeavyEnemyGameObject
    bool HeavyEnemyGameObject::FireProjectile() {
        // Heavy enemies fire less frequently but with higher damage
        if (!IsActive() || IsExploding() || !attack_cooldown_timer_.Finished()) {
            return false;
        }

        // Reset cooldown timer - longer for heavy enemies
        attack_cooldown_timer_.Start(3.0f);

        return true;
    }


    void HeavyEnemyGameObject::Update(double delta_time) {
        // Handle charging state
        if (is_charging_ && charge_timer_.Finished()) {
            is_charging_ = false;
            // Start cooldown period after charge
            charge_cooldown_.Start(3.0f);
        }

        // Call parent class update method but override if charging
        if (is_charging_) {
            // When charging, just move in straight line towards charge target
            glm::vec3 position = GetPosition();
            glm::vec3 direction = charge_target_ - position;

            if (glm::length(direction) > 0.1f) {
                direction = glm::normalize(direction);
                velocity_ = direction * charge_speed_;
                position += velocity_ * static_cast<float>(delta_time);
                SetPosition(position);

                // Update rotation to face charge direction
                SetRotation(atan2(direction.y, direction.x));
            }

            // Call parent update for explosion checks etc.
            GameObject::Update(delta_time);
        }
        else {
            // Normal enemy behavior when not charging
            EnemyGameObject::Update(delta_time);
        }
    }

    void HeavyEnemyGameObject::UpdateAttackMovement(double delta_time) {
        // Get current position
        glm::vec3 position = GetPosition();

        // Calculate direction and distance to target
        glm::vec3 direction = target_position_ - position;
        float distance_to_target = glm::length(direction);

        // Normalize direction if it's valid
        if (distance_to_target > 0.001f) {
            direction = glm::normalize(direction);

            // IMPORTANT: Always face the player when not charging
            // This ensures weapons always fire toward the player
            if (!is_charging_) {
                SetRotation(atan2(direction.y, direction.x));
            }
        }

        // Handle charging state with physics
        if (is_charging_) {
            // Continue charge movement
            glm::vec3 charge_dir = charge_target_ - position;
            float charge_distance = glm::length(charge_dir);

            // If close to charge target or very close to player, end charge early
            if (charge_distance < 0.5f || distance_to_target < 2.0f) {
                is_charging_ = false;
                charge_cooldown_.Start(4.0f);
                // Reset velocity when ending charge
                velocity_ = glm::vec3(0.0f);
                return;
            }

            // Continue charge with physics-based movement
            if (charge_distance > 0.1f) {
                // Calculate charge direction
                charge_dir = glm::normalize(charge_dir);

                // Set acceleration in charge direction
                acceleration_ = charge_dir * charge_speed_ * 2.0f;

                // Update velocity
                velocity_ += acceleration_ * static_cast<float>(delta_time);

                // Limit charge velocity
                float max_charge_speed = charge_speed_ * 1.2f;
                float current_speed = glm::length(velocity_);
                if (current_speed > max_charge_speed) {
                    velocity_ = velocity_ * (max_charge_speed / current_speed);
                }

                // Update position
                position += velocity_ * static_cast<float>(delta_time);
                SetPosition(position);

                // While charging, face the charge direction
                SetRotation(atan2(charge_dir.y, charge_dir.x));
            }
            return;
        }

        // Consider starting a charge
        if (!is_charging_ && charge_cooldown_.Finished() && CanAttack()) {
            // Only charge if player is at a good distance
            if (distance_to_target > 4.0f && distance_to_target < 12.0f) {
                is_charging_ = true;
                charge_timer_.Start(0.8f);

                // Charge to a point near the player
                float angle = atan2(direction.y, direction.x);
                float offset_angle = angle + ((rand() % 100) / 100.0f - 0.5f) * 1.5f;
                float offset_distance = distance_to_target * 0.7f;

                charge_target_ = position +
                    glm::vec3(cos(offset_angle), sin(offset_angle), 0.0f) * offset_distance;

                ResetAttackCooldown(2.0f);
                charge_cooldown_.Start(5.0f);

                std::cout << "Heavy enemy charging attack!" << std::endl;
                return;
            }
        }

        // Normal movement with physics - maintain distance from player
        float ideal_distance = GetIdealDistance();

        // Calculate distance factor for approach/retreat
        float distance_factor = CalculateDistanceFactor(distance_to_target, ideal_distance);

        // Calculate desired movement direction
        glm::vec3 movement_dir;
        if (distance_factor > 0.1f || distance_factor < -0.1f) {
            // Need to approach or retreat
            movement_dir = direction * distance_factor;
        }
        else {
            // At good distance, orbit slightly
            glm::vec3 orbit_dir(-direction.y, direction.x, 0.0f);
            movement_dir = orbit_dir * 0.5f;
        }

        // Set acceleration based on desired movement
        acceleration_ = movement_dir * speed_ * 2.0f;

        // Update velocity with physics
        velocity_ += acceleration_ * static_cast<float>(delta_time);

        // Apply maximum speed limit
        float max_speed = speed_ * 1.2f;
        float current_speed = glm::length(velocity_);
        if (current_speed > max_speed) {
            velocity_ = velocity_ * (max_speed / current_speed);
        }

        // Apply damping
        velocity_ *= 0.97f;

        // Update position
        position += velocity_ * static_cast<float>(delta_time);
        SetPosition(position);

        // Try to fire if at ideal range
        if (CanAttack() && abs(distance_to_target - ideal_distance) < ideal_distance * 0.3f) {
            FireProjectile();
        }
    }
} // namespace game