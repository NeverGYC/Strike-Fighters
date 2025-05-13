#ifndef SPRITE_H_
#define SPRITE_H_

#include "geometry.h"

namespace game {

    // A sprite (i.e., a square composed of two triangles)
    class Sprite : public Geometry {

        public:
            // Constructor and destructor
            Sprite(void);

            // Create the geometry (called once)
            void CreateGeometry(void);

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class Sprite
} // namespace game

#endif // SPRITE_H_
