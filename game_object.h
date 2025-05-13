#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "geometry.h"
#include "timer.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

    public:
        // Constructor
        GameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Update the GameObject's state. Can be overriden in children
        virtual void Update(double delta_time);

        // Renders the GameObject 
        virtual void Render(glm::mat4 view_matrix, double current_time);

        // Getters
        inline glm::vec3 GetPosition(void) const { return position_; }
        inline glm::vec2 GetScale(void) const { return scale_; }
        inline float GetRotation(void) const { return angle_; }

        virtual void PrepareForDestruction() {
            // Mark object as inactive, but don't delete anything here
            is_active_ = false;
            // Clear any temporary data that might cause memory issues
            explosion_timer_ = Timer();
            is_exploding_ = false;
        }

        // Get bearing direction (direction in which the game object
        // is facing)
        glm::vec3 GetBearing(void) const;

        // Get vector pointing to the right side of the game object
        glm::vec3 GetRight(void) const;

        // Setters
        inline void SetPosition(const glm::vec3& position) { position_ = position; }

        // Set uniform scale (for backward compatibility)
        inline void SetScale(float scale) { scale_ = glm::vec2(scale, scale); }

        // Set non-uniform scale
        inline void SetScale(const glm::vec2& scale) { scale_ = scale; }
        inline void SetScale(float x, float y) { scale_ = glm::vec2(x, y); }

        inline void SetTexture(GLuint texture) {texture_ = texture;}


        // Set ghost mode
        inline void SetGhost(bool ghost) { is_ghost_ = ghost; }
        // Get ghost mode
        inline bool IsGhost() const { return is_ghost_; }

        void SetRotation(float angle);

        // Explosion and state management
        bool IsActive() const { return is_active_; }
        bool IsExploding() const { return is_exploding_; }
        void SetActive(bool active) { is_active_ = active; }
        void SetExplosionTexture(GLuint explosion_tex) { explosion_texture_ = explosion_tex; }
        void StartExplosion();



    protected:
        // Object's Transform Variables
        glm::vec3 position_;
        glm::vec2 scale_;  // 2D scale (x, y) for non-uniform scaling
        float angle_;

        // Geometry
        Geometry* geometry_;

        // Shader
        Shader* shader_;

        // Object's texture references
        GLuint texture_;
        GLuint original_texture_;
        GLuint explosion_texture_;

        // Object state
        bool is_active_;
        bool is_exploding_;
        bool is_ghost_;    // Ghost mode for grayscale rendering
        Timer explosion_timer_;

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_