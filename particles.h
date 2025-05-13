#ifndef PARTICLES_H_
#define PARTICLES_H_

#include "geometry.h"

namespace game {

    // A set of particles that can be rendered
    class Particles : public Geometry {

        public:
            Particles(void);

            // Create the geometry (called once)
            void CreateGeometry(int num_particles);

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class Particles
} // namespace game

#endif // PARTICLES_H_
