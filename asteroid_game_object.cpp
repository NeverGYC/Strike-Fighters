#include "asteroid_game_object.h"

namespace game {
    Asteroid::Asteroid(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : GameObject(position, geom, shader, texture) {
        // Generate random properties for variety
        force_ = 2.0f + ((float)rand() / RAND_MAX) * 8.0f;  // Random bounce force between 2.0 and 10.0
        SetScale(force_ * 0.8f);  // Size proportional to bounce force
        rotation_speed_ = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;  // Random rotation speed (-1.0 to 1.0)
    }

    void Asteroid::Update(double delta_time) {
        // Call parent update
        GameObject::Update(delta_time);

        // Apply rotation over time
        float current_angle = GetRotation();
        float new_angle = current_angle + rotation_speed_ * (float)delta_time;
        SetRotation(new_angle);
    }

    void Asteroid::CollisionWithPlayer(GameObject* player) {
        if (!player || !player->IsActive()) {
            return; // Safety check
        }

        // Get positions
        glm::vec3 asteroid_pos = GetPosition();
        glm::vec3 player_pos = player->GetPosition();

        // Calculate vector from asteroid to player
        glm::vec3 push_direction = player_pos - asteroid_pos;
        float distance = glm::length(push_direction);

        // Calculate object radii
        float asteroid_radius = GetScale().x * 0.4f;
        float player_radius = player->GetScale().x * 0.4f;

        // Calculate overlap
        float overlap = (asteroid_radius + player_radius) - distance;

        // Only handle significant overlap
        if (overlap > 0.1f) {
            // Normalize the push direction (safely)
            if (distance > 0.001f) {
                push_direction /= distance;
            }
            else {
                // If too close, use a default direction
                push_direction = glm::vec3(1.0f, 0.0f, 0.0f);
            }

            // First move player out of the asteroid to prevent sticking
            player_pos = asteroid_pos + push_direction * (asteroid_radius + player_radius + 0.1f);
            player->SetPosition(player_pos);

            // Apply bounce physics
            PlayerGameObject* player_obj = dynamic_cast<PlayerGameObject*>(player);
            if (player_obj) {
                // Get player's current speed
                float player_speed = glm::length(player_obj->GetVelocity());

                // Base force is determined by asteroid's force property
                float base_force = force_ * 2.0f;

                // Add speed component - faster collision = stronger bounce
                float speed_bonus = std::min(player_speed * 0.5f, 5.0f);

                // Calculate final bounce magnitude
                float bounce_magnitude = base_force + speed_bonus;

                // Create bounce force vector in the direction away from asteroid
                glm::vec3 bounce_force = push_direction * bounce_magnitude;

                // Add a small random sideways component for variety
                glm::vec3 perpendicular(-push_direction.y, push_direction.x, 0.0f);
                float random_factor = ((float)rand() / RAND_MAX) * 0.4f - 0.2f; // -0.2 to 0.2
                bounce_force += perpendicular * base_force * random_factor;

                // Debug output
                std::cout << "Asteroid kick! Speed: " << player_speed
                    << ", Bounce force: " << glm::length(bounce_force) << std::endl;

                // Apply the bounce
                player_obj->StartSliding(bounce_force);

                // Lock player input briefly - longer for stronger bounces
                float lock_time = 0.2f + (bounce_magnitude / 20.0f); // 0.2-0.7 seconds
                player_obj->LockInputForTime(lock_time);
            }
        }
    }
} // namespace game