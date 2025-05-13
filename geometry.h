#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#define GLEW_STATIC
#include <GL/glew.h>

namespace game {

    // A piece of geometry
    class Geometry {

        public:
            // Constructor and destructor
            Geometry(void) {};

            // Create the geometry (called once)
            virtual void CreateGeometry(void) {};

            // Use the geometry
            virtual void SetGeometry(GLuint shader_program) {};

            // Getter
            int GetSize(void) const { return size_; }

        protected:
            // Geometry buffers
            GLuint vbo_;
            GLuint ebo_;
            int size_;

    }; // class Geometry
} // namespace game

#endif // GEOMETRY_H_
