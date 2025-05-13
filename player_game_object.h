#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "asteroid_game_object.h"

#include "collectible_game_object.h"
#include "timer.h"
#include <GLFW/glfw3.h>

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

    public:
        // Define WeaponType enum in the public section
        enum WeaponType {
            LASER,
            MISSILE
        };

        // Constructor
        PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update function for moving the player object around
        void Update(double delta_time) override;

        float GetHealth() const;


        int GetDamagePoints() const { return damage_points_; }

        void TakeDamage(float damage);

        void StartSliding(const glm::vec3& initial_velocity);

        void LockInputForTime(float seconds);

        bool IsSliding() const { return is_sliding_; }

        glm::vec3 GetSlidingVelocity() const { return sliding_velocity_; }

        void ApplyAcceleration(const glm::vec3& accel) {
            acceleration_ += accel;
        }


        // collectible handling
        bool CollectItem(Type type);  // Match the implemented version

        // Invincibility handling
        bool IsInvincible() const { return is_invincible_; }

        // Set invincibility flag and start timer
        void SetInvincibleTexture(GLuint invincible_tex) { invincible_texture_ = invincible_tex; }

        const Timer& GetInvincibilityTimer() const { return invincibility_timer_; }

        // Physics movement
        const glm::vec3& GetVelocity() const { return velocity_; }
        float GetMaxVelocity() const { return max_velocity_; }

        // Firing
        bool Fire();            // Returns true if a projectile was fired
        void SetProjectileTexture(GLuint texture) { projectile_texture_ = texture; }

        void SwitchWeapon();
        WeaponType GetCurrentWeapon() const { return current_weapon_; }
        void SetMissileTexture(GLuint texture) { missile_texture_ = texture; }
        bool FireMissile();  // Similar to Fire() but for missiles
        inline float GetSpeedBuff() const { return speed_buff_; }
        inline float GetPowerBuff() const { return power_buff_; }



    private:
        // Handle player input for physics movement
        void HandleInput(double delta_time);

        // Update player physics
        void UpdatePhysics(double delta_time);

        float current_health_; // or an appropriate health member


        // player state variables
        int damage_points_;             // Number of times the player can be hit
        int collected_items_;           // Number of items collected  
        bool is_invincible_;            // Flag to indicate if player is invincible
        GLuint normal_texture_;         // Texture to use when not invincible
        GLuint invincible_texture_;     // Texture to use when invincible
        Timer invincibility_timer_;     // Timer to control invincibility duration
        float speed_buff_;
        float power_buff_;
        Timer speed_timer;
        Timer power_timer;




        // Physics variables
        glm::vec3 velocity_;            // Current velocity vector
        glm::vec3 acceleration_;        // Current acceleration vector
        float acceleration_value_;      // Base acceleration rate
        float rotation_speed_;          // Rotation speed in radians per second
        float max_velocity_;            // Maximum velocity magnitude
        bool is_sliding_;
        glm::vec3 sliding_velocity_;
        bool input_locked_;
        Timer input_lock_timer_;

        // firing variables
        Timer firing_cooldown_;         // Timer for firing cooldown
        GLuint projectile_texture_;     // Texture to use for projectiles
        bool can_fire_;                 // Flag to indicate if player can fire
        

        float max_health_;              // Maximum player health (10)


        WeaponType current_weapon_;
        GLuint missile_texture_;
        Timer weapon_switch_cooldown_;
    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_