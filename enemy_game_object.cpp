#include <cmath>
#include <algorithm>
#include "enemy_game_object.h"

namespace game {

    EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : GameObject(position, geom, shader, texture) {
        // Initialize enemy-specific variables
        state_ = EnemyState::PATROLLING;
        patrol_center_ = position;
        patrol_width_ = 2.0f;
        patrol_height_ = 1.0f;
        patrol_angle_ = 0.0f;
        speed_ = 1.5f;
        detection_range_ = 8.0f;

        // Initialize health and damage variables
        health_ = 1.0f;
        max_health_ = 1.0f;
        damage_ = 1.0f;
        enemy_type_ = 0; // Base type

        // Initialize physics variables
        velocity_ = glm::vec3(0.0f);
        acceleration_ = glm::vec3(0.0f);

        // Start the course correction timer
        course_correction_timer_.Start(2.0f);  // Update every 2 seconds

        // Start attack cooldown timer
        attack_cooldown_timer_.Start(1.0f);    // Can attack every 1 second
    }

    float EnemyGameObject::GetIdealDistance() const {
        // Base ideal distances by enemy type
        switch (enemy_type_) {
        case 0: return 2.5f;  // Regular enemy
        case 1: return 1.8f;  // Fast enemy
        case 2: return 3.5f;  // Heavy enemy
        default: return 2.0f;
        }
    }

    float EnemyGameObject::CalculateDistanceFactor(float current_distance, float ideal_distance) const {
        // Calculate approach/retreat factor based on current distance
        if (current_distance < ideal_distance * 0.7f) {
            // Too close - retreat
            return -1.0f;
        }
        else if (current_distance > ideal_distance * 1.3f) {
            // Too far - approach
            return 1.0f;
        }
        else {
            // At good range - maintain distance with slight approach/retreat
            return (current_distance - ideal_distance) / ideal_distance;
        }
    }



    void EnemyGameObject::SetPatrolParameters(const glm::vec3& center, float width, float height) {
        patrol_center_ = center;
        patrol_width_ = width;
        patrol_height_ = height;
    }

    // make the enemy fire a projectile
    bool EnemyGameObject::FireProjectile() {
        // Don't fire if not active or already exploding
        if (!IsActive() || IsExploding()) {
            return false;
        }

        // Only fire if attack cooldown has finished
        if (!attack_cooldown_timer_.Finished()) {
            return false;
        }

        // Reset cooldown timer
        attack_cooldown_timer_.Start(2.0f); // Base cooldown of 2 seconds

        // Return true to indicate a projectile should be created
        return true;
    }



    void EnemyGameObject::UpdatePatrolMovement(double delta_time) {
        // Update patrol angle
        patrol_angle_ += delta_time * speed_ * 0.5f;  // Adjust speed as needed

        // Calculate new position using parametric equations for ellipse
        float x = patrol_center_.x + patrol_width_ * cos(patrol_angle_);
        float y = patrol_center_.y + patrol_height_ * sin(patrol_angle_);

        // Update position
        SetPosition(glm::vec3(x, y, 0.0f));

        // Update rotation to face movement direction
        float next_angle = patrol_angle_ + 0.1f;  // Look slightly ahead
        float dx = -patrol_width_ * sin(next_angle);
        float dy = patrol_height_ * cos(next_angle);
        SetRotation(atan2(dy, dx));
    }



    void EnemyGameObject::UpdateInterceptionMovement(double delta_time) {
        // Get direction to target
        glm::vec3 to_target = target_position_ - GetPosition();
        float distance = glm::length(to_target);

        // Always face the player when intercepting
        if (distance > 0.001f) {
            glm::vec3 direction = glm::normalize(to_target);
            // Set rotation to face target directly
            SetRotation(atan2(direction.y, direction.x));
        }

        // Get ideal distance for this enemy type
        float ideal_distance = GetIdealDistance();

        // Calculate distance factor for approach/retreat
        float distance_factor = CalculateDistanceFactor(distance, ideal_distance);

        // Calculate desired acceleration
        glm::vec3 desired_direction;
        if (distance > 0.001f) {
            desired_direction = glm::normalize(to_target);
        }
        else {
            desired_direction = glm::vec3(0.0f);
        }

        // Apply acceleration based on distance factor
        acceleration_ = desired_direction * speed_ * distance_factor * 2.0f;

        // Update velocity with physics (apply acceleration)
        velocity_ += acceleration_ * (float)delta_time;

        // Apply maximum speed limit
        float max_speed = speed_ * 1.5f;
        float current_speed = glm::length(velocity_);
        if (current_speed > max_speed) {
            velocity_ = velocity_ * (max_speed / current_speed);
        }

        // Apply damping when needed
        if (glm::length(acceleration_) < 0.1f) {
            velocity_ *= 0.98f;
        }

        // Update position with velocity
        glm::vec3 new_position = GetPosition() + velocity_ * (float)delta_time;
        SetPosition(new_position);
    }

    void EnemyGameObject::TakeDamage(float damage) {
        health_ -= damage;
        if (health_ <= 0) {
            // Begin explosion animation
            StartExplosion();
        }
    }

    void EnemyGameObject::SetState(EnemyState new_state) {
        if (state_ != new_state) {
            state_ = new_state;
            // When changing to intercepting, set initial velocity immediately
            if (state_ == EnemyState::INTERCEPTING) {
                // Calculate initial direction to target
                glm::vec3 direction = target_position_ - GetPosition();
                if (glm::length(direction) > 0.0001f) {
                    direction = glm::normalize(direction);
                    // Increase speed when intercepting for more aggressive enemies
                    velocity_ = direction * (speed_ * 1.5f);
                }
                // Start timer for next course correction with faster updates
                course_correction_timer_.Start(1.0f);  // Update direction more frequently
            }
            else if (state_ == EnemyState::ATTACKING) {
                // Reset attack cooldown when entering attack state
                attack_cooldown_timer_.Start(1.0f);
            }
        }
    }

    void EnemyGameObject::Update(double delta_time) {
        // Don't update if inactive or exploding
        if (!IsActive() || IsExploding()) {
            GameObject::Update(delta_time);
            return;
        }

        // Always update rotation to face target if in ATTACKING or INTERCEPTING state
        if ((state_ == EnemyState::ATTACKING || state_ == EnemyState::INTERCEPTING) &&
            glm::length(target_position_ - GetPosition()) > 0.001f) {

            glm::vec3 direction = glm::normalize(target_position_ - GetPosition());
            SetRotation(atan2(direction.y, direction.x));
        }

        // Update movement based on current state
        switch (state_) {
        case EnemyState::PATROLLING:
            UpdatePatrolMovement(delta_time);
            break;
        case EnemyState::INTERCEPTING:
            UpdateInterceptionMovement(delta_time);
            break;
        case EnemyState::ATTACKING:
            UpdateAttackMovement(delta_time);
            break;
        }

        // Update parent class
        GameObject::Update(delta_time);
    }
}