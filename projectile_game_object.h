#ifndef PROJECTILE_GAME_OBJECT_H_
#define PROJECTILE_GAME_OBJECT_H_

#include "game_object.h"
#include "timer.h"
#include <glm/gtc/constants.hpp>



namespace game {

    // Projectile class derived from GameObject
    class Projectile : public GameObject {
    public:
        // Constructor
        Projectile(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, const glm::vec3& direction);

        // Update function for projectile movement
        void Update(double delta_time) override;

        // Check if the projectile is expired
        bool IsExpired() const { return is_expired_; }

        // Getters for direction and speed (needed for ray-circle collision)
        const glm::vec3& GetDirection() const { return direction_; }
        float GetSpeed() const { return speed_; }
        //void Render(glm::mat4 view_matrix, double current_time) override;

		// Setters for direction and speed
        void SetDirection(const glm::vec3& direction) { direction_ = glm::normalize(direction); }


        void SetIsEnemyProjectile(bool is_enemy) { is_enemy_projectile_ = is_enemy; }
        bool IsEnemyProjectile() const { return is_enemy_projectile_; }
        void SetDamage(float damage) { damage_ = damage; }
        float GetDamage() const { return damage_; }



    private:
        glm::vec3 direction_;    // Direction of travel
        float speed_;            // Speed of the projectile
        Timer lifespan_timer_;   // Timer to track lifespan
        bool is_expired_;        // Flag to indicate if projectile has expired
        bool is_enemy_projectile_; // Flag to identify enemy projectiles
        float damage_ = 1.0f;      // Default damage amount

    };

} // namespace game

#endif // PROJECTILE_GAME_OBJECT_H_