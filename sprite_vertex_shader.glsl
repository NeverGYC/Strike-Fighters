// Source code of vertex shader
#version 130

// Vertex buffer
in vec2 vertex;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 transformation_matrix;
uniform mat4 view_matrix;
uniform float texture_scale = 1.0;


// Attributes forwarded to the fragment shader
out vec4 color_interp;
out vec2 uv_interp;

void main()
{
    uv_interp = uv * texture_scale;


    // Transform vertex
    vec4 vertex_pos = vec4(vertex, 0.0, 1.0);
    gl_Position = view_matrix * transformation_matrix * vertex_pos;
    
    // Pass attributes to fragment shader
    color_interp = vec4(color, 1.0);
    uv_interp = uv;
}
