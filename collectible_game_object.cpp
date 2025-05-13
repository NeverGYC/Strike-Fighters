#include "collectible_game_object.h"

namespace game {

    CollectibleGameObject::CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, Type type)
        : GameObject(position, geom, shader, texture), type_(type) {
        // Initialize collectible-specific variables
        is_collected_ = false;
        rotation_speed_ = 2.0f; // Rotation speed in radians per second
        // Make collectibles smaller than other objects
        SetScale(0.5f);
    }

    void CollectibleGameObject::Update(double delta_time) {
        // If already collected but still visible, check timer
        if (is_collected_) {
            // Check if it's time to make the object disappear
            if (disappear_timer_.Finished()) {
                SetActive(false);  // Now make the object disappear
            }
            // Still render as ghost, but don't rotate
            GameObject::Update(delta_time);
            return;
        }

        // Regular update for uncollected items
        float current_rotation = GetRotation();
        SetRotation(current_rotation + rotation_speed_ * delta_time);
        GameObject::Update(delta_time);
    }

    // Method to handle collection
    void CollectibleGameObject::Collect() {
        // Set flag and enable ghost mode but keep object visible
        is_collected_ = true;
        SetGhost(true);  // Enable ghost mode
        // Don't set active to false yet so we can see the ghost effect
        // Keep it visible for a short time before fully removing it

        // Start a timer to remove the object after showing ghost effect
        disappear_timer_.Start(2.0f);  // Show ghost effect for 2 seconds
    }

} // namespace game