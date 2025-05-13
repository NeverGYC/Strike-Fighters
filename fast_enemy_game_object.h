// fast_enemy_game_object.h
#ifndef FAST_ENEMY_GAME_OBJECT_H_
#define FAST_ENEMY_GAME_OBJECT_H_

#include "enemy_game_object.h"

namespace game {

    class FastEnemyGameObject : public EnemyGameObject {
    public:
        FastEnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Override parent methods
        void Update(double delta_time) override;
        
        bool FireProjectile() override;
        // Override attack behavior


    protected:
        // Override attack behavior
        void UpdateAttackMovement(double delta_time) override;

        // Fast enemy specific members
        float attack_speed_multiplier_;    // Speed increases during attack

        // Timer for strafing movement
        Timer strafe_timer_;
        bool strafe_direction_;           // True = right, False = left
    };

} // namespace game

#endif // FAST_ENEMY_GAME_OBJECT_H_