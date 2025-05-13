#include "player_game_object.h"

namespace game {

    /*
        PlayerGameObject inherits from GameObject
        It overrides GameObject's update method, so that you can check for input to change the velocity of the player
    */
    PlayerGameObject::PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : GameObject(position, geom, shader, texture) {
        // Initialize player-specific variables
        damage_points_ = 0;
        max_health_ = 20;
        current_health_ = max_health_;
        collected_items_ = 0;
        is_invincible_ = false;
        normal_texture_ = texture;
        invincible_texture_ = texture;
        speed_buff_ = 1.0f;  // Initialize speed buff
        power_buff_ = 1.0f;  // Initialize power buff

        current_weapon_ = LASER;
        missile_texture_ = texture;
        weapon_switch_cooldown_.Start(0.0f);

        // initialize physics variables - increase these values
        velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
        acceleration_ = glm::vec3(0.0f, 0.0f, 0.0f);
        acceleration_value_ = 2.0f;  // Increased from 3.5f
        max_velocity_ = 3.0f;       // Increased from 4.0f 
        rotation_speed_ = 2.5f;

        // initialize firing variables
        projectile_texture_ = texture;
        can_fire_ = true;

        is_sliding_ = false;
        input_locked_ = false;
        sliding_velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void PlayerGameObject::StartSliding(const glm::vec3& initial_velocity) {
        // Safety check - don't allow extreme values
        float max_speed = 20.0f;
        float speed = glm::length(initial_velocity);

        if (speed > 0.0001f) {
            sliding_velocity_ = initial_velocity;
            if (speed > max_speed) {
                sliding_velocity_ = (initial_velocity / speed) * max_speed;
            }
            is_sliding_ = true;

            // Reset normal movement variables
            velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
            acceleration_ = glm::vec3(0.0f, 0.0f, 0.0f);

            std::cout << "Player bounced with force: " << glm::length(sliding_velocity_) << std::endl;
        }
        else {
            // If we get a zero velocity, don't enter sliding state
            is_sliding_ = false;
            sliding_velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

    // Add the implementation for LockInputForTime
    void PlayerGameObject::LockInputForTime(float seconds) {
        input_locked_ = true;
        input_lock_timer_.Start(seconds);
    }


    void PlayerGameObject::Update(double delta_time) {
        // Add to the beginning of the Update method
        if (speed_buff_ > 1.0f && speed_timer.Finished()) {
            speed_buff_ = 1.0f;
            std::cout << "Speed buff wore off!" << std::endl;
        }

        if (power_buff_ > 1.0f && power_timer.Finished()) {
            power_buff_ = 1.0f;
            std::cout << "Power buff wore off!" << std::endl;
        }

        // Check if explosion timer finished
        if (is_exploding_ && explosion_timer_.Finished() && is_active_) {
            is_active_ = false;  // Deactivate the player

            // Print game over message with clear formatting
            std::cout << "\n=========================" << std::endl;
            std::cout << "      GAME OVER!" << std::endl;
            std::cout << "=========================" << std::endl;
            std::cout << "Player destroyed after 3 hits!" << std::endl;

            // Close the window
            glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
            return;
        }

        // Check if input lock has expired
        if (input_locked_ && input_lock_timer_.Finished()) {
            input_locked_ = false;
        }

        // In PlayerGameObject::Update, ensure the sliding code is robust:
        if (is_sliding_) {
            // Apply a position update with bounds checking
            glm::vec3 new_pos = GetPosition() + sliding_velocity_ * (float)delta_time;

            // Add position bounds if needed
            const float BOUND = 20.0f;
            if (abs(new_pos.x) <= BOUND && abs(new_pos.y) <= BOUND) {
                SetPosition(new_pos);
            }
            else {
                // If hit boundary, reverse sliding velocity component (bounce)
                if (abs(new_pos.x) > BOUND) sliding_velocity_.x = -sliding_velocity_.x * 0.5f;
                if (abs(new_pos.y) > BOUND) sliding_velocity_.y = -sliding_velocity_.y * 0.5f;
            }

            // Apply gradual damping
            sliding_velocity_ *= 0.97f;

            // Stop sliding when velocity becomes negligible
            if (glm::length(sliding_velocity_) < 0.05f) {
                is_sliding_ = false;
                sliding_velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        }

        // check if invincibility timer finished
        if (is_invincible_ && invincibility_timer_.Finished()) {
            is_invincible_ = false;
            texture_ = normal_texture_;
            std::cout << "Invincibility wore off!" << std::endl;
        }


        // Check if firing cooldown has finished
        if (!can_fire_ && firing_cooldown_.Finished()) {
            can_fire_ = true;
        }

        // NOTE: We're NOT calling HandleInput here anymore since Game::HandleControls 
        // is handling the player input

        // Only update physics if not exploding, or if we are exploding,
        // just let the explosion drift with reduced speed
        if (!is_exploding_) {
            UpdatePhysics(delta_time);
        }
        else {
            // Simple drift motion for explosion - just apply velocity without acceleration
            glm::vec3 new_position = GetPosition() + velocity_ * (float)delta_time;
            SetPosition(new_position);
            // Gradually slow down
            velocity_ *= 0.97f;
        }

        // Call parent's update to handle basic explosion timer
        GameObject::Update(delta_time);
    }


    // Implement SwitchWeapon method
    void PlayerGameObject::SwitchWeapon() {

        // Verify textures are loaded before switching
        if (missile_texture_ == 0) {
            std::cout << "Warning: Cannot switch to missile weapon - texture not loaded" << std::endl;
            return;
        }

        // Check cooldown
        if (weapon_switch_cooldown_.Finished()) {
            // Toggle between weapon types
            if (current_weapon_ == LASER) {
                current_weapon_ = MISSILE;
                std::cout << "Switched to Missile Launcher" << std::endl;
            }
            else {
                current_weapon_ = LASER;
                std::cout << "Switched to Laser Cannon" << std::endl;
            }

            // Set a small cooldown to prevent rapid switching
            weapon_switch_cooldown_.Start(0.3f);
        }
    }

    // Implement FireMissile method
    bool PlayerGameObject::FireMissile() {
        // Don't allow firing if player is exploding or during cooldown
        if (is_exploding_ || !can_fire_) {
            return false;
        }

        // Start cooldown timer (0.8 seconds between missiles - longer than lasers)
        firing_cooldown_.Start(0.8f);
        can_fire_ = false;

        return true; // Indicates a missile should be created
    }


    


    void PlayerGameObject::UpdatePhysics(double delta_time) {
        // Update velocity based on acceleration (v = v0 + a*t)
        velocity_ += acceleration_ * (float)delta_time * speed_buff_; // Apply speed buff

        // Limit velocity to maximum value
        float modified_max_velocity = max_velocity_ * speed_buff_; // Modified max velocity
        float speed = glm::length(velocity_);
        if (speed > modified_max_velocity) {
            velocity_ = (velocity_ / speed) * modified_max_velocity;
        }

        // Apply drag/friction to gradually slow down when no acceleration is applied
        if (glm::length(acceleration_) < 0.1f) {
            velocity_ *= 0.99f; // Reduced from 0.98f for smoother movement
        }

        // Update position based on velocity (p = p0 + v*t)
        glm::vec3 new_position = GetPosition() + velocity_ * (float)delta_time;

        // Add position bounds if needed (optional, can be removed for free movement)
        const float BOUND = 20.0f; // Larger bound for free movement in world
        if (abs(new_position.x) <= BOUND && abs(new_position.y) <= BOUND) {
            SetPosition(new_position);
        }
        else {
            // If hit boundary, reverse velocity component (bounce)
            if (abs(new_position.x) > BOUND) velocity_.x = -velocity_.x * 0.5f;
            if (abs(new_position.y) > BOUND) velocity_.y = -velocity_.y * 0.5f;
        }

        // Reset acceleration after applying it
        acceleration_ = glm::vec3(0.0f, 0.0f, 0.0f);
    }



    void PlayerGameObject::TakeDamage(float damage) {
        // If invincible, don't take damage
        if (is_invincible_) {
            std::cout << "Damage blocked by invincibility shield!" << std::endl;
            return;
        }

        // Increase damage points (track hits taken)
        damage_points_++;

        // Decrease health by damage amount
        current_health_ -= damage;

        // Print current damage status
        std::cout << "Player took damage! Health: " << current_health_ << "/" << max_health_ << std::endl;

        // If health drops to zero or below, trigger explosion
        if (current_health_ <= 0) {
            is_exploding_ = true;
            texture_ = explosion_texture_;  // Switch to explosion texture
            explosion_timer_.Start(5.0f);   // Start 5-second explosion timer
            std::cout << "Critical damage! Player exploding..." << std::endl;
        }
    }


    bool PlayerGameObject::CollectItem(Type type) {
        switch (type) {
            // If the collectible type is speed buff
        case Type::SPEED:
            speed_buff_ = 1.5f;
            speed_timer.Start(10.0f);
            std::cout << "SPEED BUFF ACTIVATED! Duration: 10 seconds" << std::endl;
            std::cout << "Max Speed: " << max_velocity_ * speed_buff_ << ", Original Speed: " << max_velocity_ << std::endl;
            return false;

            // If the collectible type is power buff
        case Type::POWER:
            power_buff_ = 2.0f; // Add damage
            power_timer.Start(10.0f);
            std::cout << "POWER BUFF ACTIVATED! Duration: 10 seconds" << std::endl;
            return false;

        case Type::INVINCIBLE:
        default:
            collected_items_++;
            std::cout << "Collected item! Total items: " << collected_items_ << "/5" << std::endl;

            if (collected_items_ >= 5) {
                is_invincible_ = true;
                texture_ = invincible_texture_;
                invincibility_timer_.Start(10.0f);
                collected_items_ = 0;

                std::cout << "INVINCIBILITY MODE ACTIVATED! Duration: 10 seconds" << std::endl;
                return true;  // Indicates invincibility was activated
            }
            return false;  // Regular item collection
        }
    }
    
    float PlayerGameObject::GetHealth() const {
        return current_health_;
    }
    
    bool PlayerGameObject::Fire() {
        // Don't allow firing if player is exploding or during cooldown
        if (is_exploding_ || !can_fire_) {
            return false;
        }

        float cooldown = 0.5 / power_buff_; // Use power buff to reduce cooldown

        // Start cooldown timer
        firing_cooldown_.Start(cooldown);
        can_fire_ = false;

        return true; // Indicates a projectile should be created
    }


} // namespace game