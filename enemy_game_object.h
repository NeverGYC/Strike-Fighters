#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include "game_object.h"
#include "timer.h"

namespace game {

    // Enemy states
    enum class EnemyState {
        PATROLLING,
        INTERCEPTING,
        ATTACKING  // New state for attacking
    };

    class EnemyGameObject : public GameObject {
    public:
        // Constructor
        EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update function for enemy behavior
        void Update(double delta_time) override;

        // State management
        void SetState(EnemyState new_state);
        EnemyState GetState() const { return state_; }

        // Set patrol parameters
        void SetPatrolParameters(const glm::vec3& center, float width, float height);

        // Set target for interception
        void SetTarget(const glm::vec3& target) { target_position_ = target; }

        // Set detection range
        void SetDetectionRange(float range) { detection_range_ = range; }

        // Get detection range
        float GetDetectionRange() const { return detection_range_; }

        // Health and damage methods
        void SetHealth(float health) { health_ = health; max_health_ = health; }
        float GetHealth() const { return health_; }
        float GetMaxHealth() const { return max_health_; }
        void TakeDamage(float damage);

        // Attack methods
        void SetDamage(float damage) { damage_ = damage; }
        float GetDamage() const { return damage_; }

        // Attack cooldown timer
        bool CanAttack() const { return attack_cooldown_timer_.Finished(); }
        void ResetAttackCooldown(float cooldown) { attack_cooldown_timer_.Start(cooldown); }

        // Type identifier to help differentiate enemy types
        void SetEnemyType(int type) { enemy_type_ = type; }
        int GetEnemyType() const { return enemy_type_; }

        virtual bool FireProjectile(); // Returns true if a projectile was fired
        void SetProjectileTexture(GLuint texture) { projectile_texture_ = texture; }




    protected:
        // Enemy state
        EnemyState state_;

        GLuint projectile_texture_; // Texture for enemy projectiles



        virtual float GetIdealDistance() const;
        float CalculateDistanceFactor(float current_distance, float ideal_distance) const;

        glm::vec3 acceleration_;


        // Patrol parameters
        glm::vec3 patrol_center_;
        float patrol_width_;
        float patrol_height_;
        float patrol_angle_;                // Current angle in the ellipse

        // Interception parameters
        glm::vec3 target_position_;
        glm::vec3 velocity_;
        float speed_;                       // Movement speed
        float detection_range_;             // Range at which enemy detects player

        // Health and damage parameters
        float health_;                      // Current health
        float max_health_;                  // Maximum health
        float damage_;                      // Damage inflicted to player
        int enemy_type_;                    // Type identifier (1=fast, 2=heavy)

        // Timers
        Timer course_correction_timer_;     // Timer for updating course
        Timer attack_cooldown_timer_;       // Timer for attack cooldown

        // Helper functions
        void UpdatePatrolMovement(double delta_time);
        void UpdateInterceptionMovement(double delta_time);
        virtual void UpdateAttackMovement(double delta_time) {}; // Empty implementation for base class
    };

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_