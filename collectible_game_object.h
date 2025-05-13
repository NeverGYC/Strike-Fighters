#ifndef COLLECTIBLE_GAME_OBJECT_H_
#define COLLECTIBLE_GAME_OBJECT_H_

#include "game_object.h"

namespace game {
    enum class Type {
        SPEED,      // Speed-up buff
        POWER,      // Add damage
        INVINCIBLE  // Original collectible game object
    };

    class CollectibleGameObject : public GameObject {
    public:
        CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, Type type = Type::INVINCIBLE);

        // Update function for any collectible-specific behavior
        void Update(double delta_time) override;

        // Getter to check if collectible has been collected
        bool IsCollected() const { return is_collected_; }

        // Getter to check the type
        Type GetType() const { return type_; }

        // Method to handle collection
        void Collect();

    private:
        bool is_collected_;     // Flag to indicate if collectible has been collected
        float rotation_speed_;  // For simple spinning animation
        Timer disappear_timer_; // Timer for delayed disappearance after collection
        Type type_;             // The collectible object type
    };

} // namespace game

#endif // COLLECTIBLE_GAME_OBJECT_H_