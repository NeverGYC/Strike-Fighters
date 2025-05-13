#ifndef MISSILE_FLAME_H_
#define MISSILE_FLAME_H_

#include "game_object.h"
#include <vector>
#include <random>

namespace game {

    // Structure to hold data for a single flame particle
    struct FlameParticle {
        glm::vec3 offset;     // Offset from missile position
        glm::vec3 velocity;   // Velocity relative to missile
        float size;
        float alpha;
        float lifetime;
        float max_lifetime;
    };

    // A class for simulating missile flame particles
    class MissileFlame : public GameObject {
    public:
        // Constructor
        MissileFlame(const glm::vec3& position, Geometry* geom, Shader* shader,
            GLuint texture, GameObject* parent);

        // Destructor
        ~MissileFlame();

        // Override PrepareForDestruction to safely handle parent reference
        void PrepareForDestruction() override;

        // Update flame particles
        void Update(double delta_time) override;

        // Render flame particles
        void Render(glm::mat4 view_matrix, double current_time) override;

    private:
        // Initialize a new particle
        void InitParticle(FlameParticle& particle);

        GameObject* parent_;               // Parent missile
        std::vector<FlameParticle> particles_;  // Flame particles
        std::mt19937 rng_;                 // Random number generator

        // Distributions for random values
        std::uniform_real_distribution<float> dist_lifetime_;
        std::uniform_real_distribution<float> dist_offset_;
        std::uniform_real_distribution<float> dist_size_;
        std::uniform_real_distribution<float> dist_speed_;
    };

} // namespace game

#endif // MISSILE_FLAME_H_