#ifndef SPACE_DUST_H_
#define SPACE_DUST_H_

#include <vector>
#include <random>
#include "game_object.h"

namespace game {

    // Structure to hold data for a single dust particle
    struct DustParticle {
        glm::vec3 position;
        glm::vec3 velocity;
        float size;
        float alpha;
    };

    // A class for simulating space dust particles
    class SpaceDust : public GameObject {
    public:
        // Constructor
        SpaceDust(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int num_particles = 200);

        // Destructor
        ~SpaceDust();

        // Update the position of particles
        void Update(double delta_time) override;

        // Render the particles
        void Render(glm::mat4 view_matrix, double current_time) override;

        // Set the boundary for particles
        void SetBounds(float bound_x, float bound_y);

    private:
        // Initialize a dust particle
        void InitParticle(DustParticle& particle, bool randomize_pos = true);

        std::vector<DustParticle> particles_;
        int num_particles_;

        // Boundaries for particles
        float bound_x_;
        float bound_y_;

        // Random number generator
        std::mt19937 rng_;
        std::uniform_real_distribution<float> dist_pos_;
        std::uniform_real_distribution<float> dist_vel_;
        std::uniform_real_distribution<float> dist_size_;
        std::uniform_real_distribution<float> dist_alpha_;
    };

} // namespace game

#endif // SPACE_DUST_H_