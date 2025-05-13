#include "space_dust.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ctime>

namespace game {

    SpaceDust::SpaceDust(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int num_particles)
        : GameObject(position, geom, shader, texture) {

        // Initialize random number generator with current time
        rng_ = std::mt19937(static_cast<unsigned int>(time(nullptr)));

        // Set default boundaries
        bound_x_ = 40.0f;
        bound_y_ = 40.0f;

        // Initialize distributions
        dist_pos_ = std::uniform_real_distribution<float>(-bound_x_, bound_x_);
        dist_vel_ = std::uniform_real_distribution<float>(-0.5f, 0.5f);
        dist_size_ = std::uniform_real_distribution<float>(0.05f, 0.2f);
        dist_alpha_ = std::uniform_real_distribution<float>(0.3f, 0.8f);

        // Store number of particles
        num_particles_ = num_particles;

        // Initialize particles
        particles_.resize(num_particles_);
        for (auto& particle : particles_) {
            InitParticle(particle);
        }
    }

    SpaceDust::~SpaceDust() {
        // Nothing to clean up as vector handles memory management
    }

    void SpaceDust::InitParticle(DustParticle& particle, bool randomize_pos) {
        if (randomize_pos) {
            // Random position within bounds
            particle.position = glm::vec3(dist_pos_(rng_), dist_pos_(rng_), 0.0f);
        }

        // Random velocity, very slow for dust-like effect
        particle.velocity = glm::vec3(dist_vel_(rng_), dist_vel_(rng_), 0.0f);

        // Random size for variety
        particle.size = dist_size_(rng_);

        // Random alpha for depth illusion
        particle.alpha = dist_alpha_(rng_);
    }

    void SpaceDust::Update(double delta_time) {
        // Update position of each particle
        for (auto& particle : particles_) {
            // Move based on velocity
            particle.position += particle.velocity * static_cast<float>(delta_time);

            // Check if particle is out of bounds
            if (abs(particle.position.x) > bound_x_ || abs(particle.position.y) > bound_y_) {
                // Reset particle but at opposite edge for seamless effect
                float edge_x = particle.position.x > 0 ? -bound_x_ : bound_x_;
                float edge_y = particle.position.y > 0 ? -bound_y_ : bound_y_;

                // If moving horizontally out of bounds
                if (abs(particle.position.x) > bound_x_) {
                    particle.position.x = edge_x;
                    particle.position.y = dist_pos_(rng_); // Random y position
                }
                // If moving vertically out of bounds
                else {
                    particle.position.x = dist_pos_(rng_); // Random x position
                    particle.position.y = edge_y;
                }

                // Randomize other properties for variety
                InitParticle(particle, false);
            }
        }

        // Call parent's update
        GameObject::Update(delta_time);
    }

    void SpaceDust::Render(glm::mat4 view_matrix, double current_time) {
        // Enable blending for transparent particles
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Disable depth testing for particles
        glDisable(GL_DEPTH_TEST);

        // Setup shader
        shader_->Enable();

        // Set view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Set time uniform for possible animation effects
        shader_->SetUniform1f("time", static_cast<float>(current_time));

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Render each particle
        for (const auto& particle : particles_) {
            // Calculate transformation for this particle
            glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f),
                glm::vec3(particle.size, particle.size, 1.0f));
            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f),
                particle.position);
            glm::mat4 transform = translation_matrix * scaling_matrix;

            // Set transformation matrix
            shader_->SetUniformMat4("transformation_matrix", transform);

            // Set particle alpha - add to shader if needed
            if (shader_->GetShaderProgram() != 0) {
                // Try to set alpha if the shader supports it
                try {
                    shader_->SetUniform1f("alpha", particle.alpha);
                }
                catch (...) {
                    // Ignore if uniform doesn't exist
                }
            }

            // Set geometry
            geometry_->SetGeometry(shader_->GetShaderProgram());

            // Draw the particle
            glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
        }

        // Cleanup
        shader_->Disable();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    void SpaceDust::SetBounds(float bound_x, float bound_y) {
        bound_x_ = bound_x;
        bound_y_ = bound_y;

        // Update distribution ranges
        dist_pos_ = std::uniform_real_distribution<float>(-bound_x_, bound_x_);
    }

} // namespace game