#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Uniform (global) buffer
uniform sampler2D onetex;
uniform float time;
uniform float alpha;

// Output fragment color
out vec4 FragmentColor;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);
    
    // Make dust particles white with slight blue tint for space feel
    color.rgb = vec3(0.9, 0.95, 1.0);
    
    // Apply subtle color variation based on time
    vec3 tint1 = vec3(0.9, 0.95, 1.0);  // Blueish white
    vec3 tint2 = vec3(1.0, 0.98, 0.9);  // Yellowish white
    float tint_factor = 0.5 + 0.5 * sin(time * 0.2 + gl_FragCoord.x * 0.01 + gl_FragCoord.y * 0.01);
    color.rgb = mix(tint1, tint2, tint_factor);
    
    // Calculate distance from center for radial falloff
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(uv_interp, center);
    
    // Apply radial falloff
    float falloff = clamp(1.0 - dist * 2.0, 0.0, 1.0);
    
    // Apply pulsating effect based on time for subtle animation
    float pulse = 0.85 + 0.15 * sin(time * 1.5);
    
    // Apply particle alpha with pulsating effect and falloff
    color.a = color.a * alpha * pulse * falloff;
    
    // Set fragment color
    FragmentColor = color;
    
    // Discard transparent fragments
    if (color.a < 0.05) {
        discard;
    }
}