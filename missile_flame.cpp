#include "missile_flame.h"
#include "projectile_game_object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ctime>

namespace game {

    MissileFlame::MissileFlame(const glm::vec3& position, Geometry* geom, Shader* shader,
        GLuint texture, GameObject* parent)
        : GameObject(position, geom, shader, texture), parent_(parent) {

        // Initialize random number generator
        rng_ = std::mt19937(static_cast<unsigned int>(time(nullptr)));

        // Initialize distributions
        dist_lifetime_ = std::uniform_real_distribution<float>(0.1f, 0.3f);
        dist_offset_ = std::uniform_real_distribution<float>(-0.1f, 0.1f);
        dist_size_ = std::uniform_real_distribution<float>(0.05f, 0.15f);
        dist_speed_ = std::uniform_real_distribution<float>(1.0f, 2.5f);

        // Create particles
        particles_.resize(40);  // 40 particles for the flame
        for (auto& particle : particles_) {
            InitParticle(particle);
            // Randomize initial lifetime so all particles don't appear at once
            particle.lifetime = dist_lifetime_(rng_) * 0.8f;
        }
    }

    MissileFlame::~MissileFlame() {
        // Ensure parent reference is nullified in destructor
        parent_ = nullptr;
    }

    void MissileFlame::PrepareForDestruction() {
        // Clear parent reference safely
        parent_ = nullptr;

        // Call base class implementation to set inactive
        GameObject::PrepareForDestruction();
    }

    void MissileFlame::InitParticle(FlameParticle& particle) {
        // Set random lifetime
        particle.max_lifetime = dist_lifetime_(rng_);
        particle.lifetime = 0.0f;

        // Set initial offset at the back of the missile
        // Since the missile texture points right, the back is at negative x
        particle.offset = glm::vec3(-0.25f, dist_offset_(rng_) * 0.1f, 0.0f);

        // Set initial velocity (relative to missile) - away from the back
        particle.velocity = glm::vec3(-dist_speed_(rng_), dist_offset_(rng_) * 0.3f, 0.0f);

        // Set size and alpha
        particle.size = dist_size_(rng_);
        particle.alpha = 0.8f + dist_offset_(rng_) * 0.2f;  // Slightly random alpha
    }

    void MissileFlame::Update(double delta_time) {
        // Check if parent is still active
        if (!parent_ || !parent_->IsActive()) {
            // Parent is gone, deactivate flame
            SetActive(false);
            return;
        }

        // Update position to follow parent
        SetPosition(parent_->GetPosition());

        // Get parent rotation
        float parent_rotation = parent_->GetRotation();

        // Set our rotation to match parent
        SetRotation(parent_rotation);

        // Update particles
        for (auto& particle : particles_) {
            // Update lifetime
            particle.lifetime += static_cast<float>(delta_time);

            // Check if particle expired
            if (particle.lifetime >= particle.max_lifetime) {
                InitParticle(particle);
                continue;
            }

            // Update position based on velocity (in local space)
            particle.offset += particle.velocity * static_cast<float>(delta_time);

            // Update alpha based on lifetime (fade out)
            float life_factor = particle.lifetime / particle.max_lifetime;
            particle.alpha = 0.8f * (1.0f - life_factor);

            // Update size (shrink over time)
            particle.size *= (1.0f - life_factor * 0.1f);
        }

        // Call parent's update
        GameObject::Update(delta_time);
    }

    void MissileFlame::Render(glm::mat4 view_matrix, double current_time) {
        if (!IsActive() || !parent_ || !parent_->IsActive()) {
            return;
        }

        // Enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Additive blending for glow effect

        // Setup shader
        shader_->Enable();

        // Set view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Set time uniform for possible animation effects
        shader_->SetUniform1f("time", static_cast<float>(current_time));

        // Bind flame texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Get parent position and rotation
        glm::vec3 parent_pos = parent_->GetPosition();
        float parent_rot = parent_->GetRotation();

        // Create rotation matrix for parent orientation
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), parent_rot, glm::vec3(0.0f, 0.0f, 1.0f));

        // Draw each particle
        for (const auto& particle : particles_) {
            // Skip nearly transparent particles
            if (particle.alpha < 0.05f) continue;

            // Transform particle offset to world space based on missile orientation
            glm::vec4 rotated_offset = rotation_matrix * glm::vec4(particle.offset, 0.0f);
            glm::vec3 world_pos = parent_pos + glm::vec3(rotated_offset);

            // Create particle transformation
            glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f),
                glm::vec3(particle.size, particle.size, 1.0f));
            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), world_pos);

            // Add rotation to make particles face the right direction
            glm::mat4 particle_rotation = glm::rotate(glm::mat4(1.0f), parent_rot, glm::vec3(0.0f, 0.0f, 1.0f));

            // Combine transformations
            glm::mat4 transform = translation_matrix * particle_rotation * scaling_matrix;

            // Set transformation matrix
            shader_->SetUniformMat4("transformation_matrix", transform);

            // Try to set alpha if shader supports it
            try {
                shader_->SetUniform1f("alpha", particle.alpha);
            }
            catch (...) {
                // Ignore if shader doesn't have alpha uniform
            }

            // Set geometry
            geometry_->SetGeometry(shader_->GetShaderProgram());

            // Draw the particle
            glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
        }

        // Cleanup
        shader_->Disable();
        glDisable(GL_BLEND);
    }

} // namespace game