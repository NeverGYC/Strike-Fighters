#ifndef ASTEROID_GAME_OBJECT_H_
#define ASTEROID_GAME_OBJECT_H_

#include "game_object.h"
#include "player_game_object.h"

namespace game {
    class Asteroid : public GameObject {
    public:
        // Constructor
        Asteroid(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update function
        void Update(double delta_time) override;

        // To handle collision with player object
        void CollisionWithPlayer(GameObject* player);

        // Getters
        float GetForce() const { return force_; }
        float GetRotationSpeed() const { return rotation_speed_; }

        // Setters - these need to return void, not float
        void SetForce(float force) { force_ = force; }
        void SetRotationSpeed(float rotation_speed) { rotation_speed_ = rotation_speed; }

    private:
        float force_;           // Bounce force applied to players on collision
        float rotation_speed_;  // Self-rotation speed of the asteroid
    };
} // namespace game

#endif // ASTEROID_GAME_OBJECT_H_