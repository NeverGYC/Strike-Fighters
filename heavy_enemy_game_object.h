// heavy_enemy_game_object.h
#ifndef HEAVY_ENEMY_GAME_OBJECT_H_
#define HEAVY_ENEMY_GAME_OBJECT_H_

#include "enemy_game_object.h"

namespace game {

    class HeavyEnemyGameObject : public EnemyGameObject {
    public:
        HeavyEnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Override parent methods
        void Update(double delta_time) override;
        bool FireProjectile() override;


    protected:
        // Override attack behavior
        void UpdateAttackMovement(double delta_time) override;

        // Heavy enemy specific members
        float charge_speed_;        // Speed during charging attack
        bool is_charging_;          // Whether currently charging
        Timer charge_timer_;        // Timer for charge duration
        Timer charge_cooldown_;     // Cooldown between charges

        // Tracking the charge target position
        glm::vec3 charge_target_;
    };

} // namespace game

#endif // HEAVY_ENEMY_GAME_OBJECT_H_