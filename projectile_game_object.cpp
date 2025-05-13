#include "projectile_game_object.h"

namespace game {

    Projectile::Projectile(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, const glm::vec3& direction)
        : GameObject(position, geom, shader, texture) {

        // Initialize projectile properties
        direction_ = glm::normalize(direction); // Ensure direction is a unit vector
        speed_ = 8.0f;  // Projectiles move fast
        is_expired_ = false;

        // Set smaller scale for projectiles
        SetScale(0.3f);

        // Set rotation to match direction
        SetRotation(atan2(direction_.y, direction_.x) - glm::pi<float>() / 2.0f);

        // Start the lifespan timer - projectiles live for 2 seconds
        lifespan_timer_.Start(2.0f);

        // Initialize as player projectile by default
        is_enemy_projectile_ = false;
    }

    void Projectile::Update(double delta_time) {
        // Check if the projectile has expired
        if (lifespan_timer_.Finished()) {
            is_expired_ = true;
            SetActive(false);
            return;
        }

        // Update position based on direction and speed
        glm::vec3 position = GetPosition();
        position += direction_ * speed_ * (float)delta_time;
        SetPosition(position);

        // Call parent's update method
        GameObject::Update(delta_time);
    }
    
    //void Projectile::Render(glm::mat4 view_matrix, double current_time) {
    //    // Adjust rotation temporarily for rendering
    //    float saved_angle = angle_;
    //    angle_ -= glm::pi<float>() / 2.0f;  // Offset by -90 degrees

    //    // Call parent render method
    //    GameObject::Render(view_matrix, current_time);

    //    // Restore original angle
    //    angle_ = saved_angle;
    //}


} // namespace game