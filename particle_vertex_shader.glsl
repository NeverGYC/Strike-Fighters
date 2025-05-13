// particle_vertex_shader.glsl
#version 130

// Vertex buffer
in vec2 vertex;
in vec2 dir;
in float t;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 transformation_matrix;
uniform mat4 view_matrix;
uniform float time;

// Attributes forwarded to the fragment shader
out vec2 uv_interp;

void main()
{
    // Apply time to compute the actual position of vertex
    // (dir is a velocity vector and it should be multiplied by time)
    float dt = sin(time*3.0 + t*6.28) * 0.1;
    vec4 vertex_pos = vec4(vertex, 0.0, 1.0);
    vertex_pos.x = vertex_pos.x + dir.x*dt*10.0;
    vertex_pos.y = vertex_pos.y + dir.y*dt*10.0;

    // Transform vertex
    gl_Position = view_matrix * transformation_matrix * vertex_pos;
    
    // Pass attributes to fragment shader
    uv_interp = uv;
}