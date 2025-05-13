// homing_missile.h
#ifndef HOMING_MISSILE_H_
#define HOMING_MISSILE_H_

#include "projectile_game_object.h"
#include "timer.h"

namespace game {

    class HomingMissile : public Projectile {
    public:
        // Constructor
        HomingMissile(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture,
            const glm::vec3& direction);

        // Override update method to implement homing behavior
        void Update(double delta_time) override;

        // Set the target to track
        void SetTarget(GameObject* target) { target_ = target; }

        // Get damage value (lower than regular projectile)
        float GetDamage() const { return damage_; }

    private:
        GameObject* target_;           // The current tracking target
        float turning_speed_;          // How fast the missile can turn
        float damage_;                 // Damage value (less than regular projectile)
        Timer retarget_timer_;         // Timer for finding new targets if current is invalid
    };

} // namespace game

#endif // HOMING_MISSILE_H_