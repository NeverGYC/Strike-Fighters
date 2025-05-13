#ifndef MINIMAP_SPRITE_H_
#define MINIMAP_SPRITE_H_

#include "geometry.h"
#include <iostream>
namespace game {

    // A simple sprite for the minimap
    class MinimapSprite : public Geometry {

    public:
        // Constructor
        MinimapSprite(void) : Geometry() {
            vbo_ = 0;
            ebo_ = 0;
            size_ = 0;
        }

        // Create the geometry (called once)
        void CreateGeometry(void) {
            // Very simple quad with minimal attributes
            GLfloat vertex[] = {
                // Position      Color                Texture coordinates
                -0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f, // Top-left
                 0.5f,  0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, // Top-right
                 0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
                -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f  // Bottom-left
            };

            // Two triangles referencing the vertices
            GLuint face[] = {
                0, 1, 2, // t1
                2, 3, 0  // t2
            };

            // Create buffer for vertices
            glGenBuffers(1, &vbo_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

            // Create buffer for faces (index buffer)
            glGenBuffers(1, &ebo_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

            // Set number of elements in array buffer
            size_ = sizeof(face) / sizeof(GLuint);
        }

        // Use the geometry
        void SetGeometry(GLuint shader_program) {
            // No complex blending
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Bind buffers
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

            // Verify shader program is valid
            if (shader_program == 0) {
                std::cerr << "ERROR: Invalid shader program in MinimapSprite::SetGeometry" << std::endl;
                return;
            }

            // Set attributes for shaders - simple version to avoid errors
            GLint vertex_att = glGetAttribLocation(shader_program, "vertex");
            if (vertex_att >= 0) {
                glVertexAttribPointer(vertex_att, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
                glEnableVertexAttribArray(vertex_att);
            }

            GLint color_att = glGetAttribLocation(shader_program, "color");
            if (color_att >= 0) {
                glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
                glEnableVertexAttribArray(color_att);
            }

            GLint tex_att = glGetAttribLocation(shader_program, "uv");
            if (tex_att >= 0) {
                glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
                glEnableVertexAttribArray(tex_att);
            }
        }
    };

} // namespace game

#endif // MINIMAP_SPRITE_H_