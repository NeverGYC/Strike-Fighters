// particle_fragment_shader.glsl
#version 130

// Attributes passed from the vertex shader
in vec2 uv_interp;

// Uniform (global) buffer
uniform sampler2D onetex;
uniform float time;
uniform float alpha = 1.0; // Default alpha value

// Un-modulated color of fragment
out vec4 FragmentColor;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);
    
    // Add time-based glow effect
    float glow = 0.6 + 0.4 * sin(time * 3.0);
    
    // Apply glow effect to color and alpha
    color.rgb *= glow;
    color.a *= alpha;
    
    // For flame particles - add orange/yellow gradient
    vec3 flame_color = vec3(1.0, 0.6 + 0.4 * sin(time * 5.0), 0.2);
    color.rgb = mix(color.rgb, flame_color, 0.6);
    
    // Set fragment color
    FragmentColor = color;
    
    // Discard very transparent fragments
    if (color.a < 0.05) {
        discard;
    }
}