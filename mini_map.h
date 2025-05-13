#ifndef MINI_MAP_H_
#define MINI_MAP_H_

#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
#include "geometry.h"
#include "minimap_sprite.h"

namespace game {

    // Forward declarations
    class GameObject;
    class PlayerGameObject;
    class EnemyGameObject;
    class BossGameObject;

    // Class for rendering a mini map in the corner of the screen
    class MiniMap {
    public:
        // Constructor
        MiniMap();

        // Initialize the mini map
        void Init(Geometry* geom, Shader* shader);

        // Render the mini map
        void Render(const std::vector<GameObject*>& game_objects, BossGameObject* boss);

        // Set the map position and size
        void SetPosition(float x, float y) { pos_x_ = x; pos_y_ = y; }
        void SetSize(float width, float height) { width_ = width; height_ = height; }

        // Set the world bounds that the mini map represents
        void SetWorldBounds(float bound_x, float bound_y) {
            world_bound_x_ = bound_x;
            world_bound_y_ = bound_y;
        }

        // Get info about the mini map
        float GetWidth() const { return width_; }
        float GetHeight() const { return height_; }

    private:

        // Position and size of the mini map on screen
        float pos_x_;
        float pos_y_;
        float width_;
        float height_;

        // World boundaries for scaling
        float world_bound_x_;
        float world_bound_y_;

        void DrawMapDot(const glm::vec3& world_pos, const glm::vec3& color, float size);


        // Geometry and shader for rendering
        Geometry* geometry_;
        Shader* shader_;

        // Colors for different entities
        glm::vec3 player_color_;
        glm::vec3 enemy_color_;
        glm::vec3 boss_color_;
        glm::vec3 background_color_;
    };

} // namespace game

#endif // MINI_MAP_H_