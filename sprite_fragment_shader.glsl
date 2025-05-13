// Source code of fragment shader
#version 130
// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;
// Texture sampler
uniform sampler2D onetex;
// Ghost mode flag
uniform int ghost_mode;
// Color override for minimap
uniform vec3 color_override;
uniform int use_color_override = 0;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);
    
    // Apply ghost mode (grayscale) if enabled
    if (ghost_mode == 1) {
        // Convert to grayscale using luminance formula
        float luminance = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
        color = vec4(luminance, luminance, luminance, color.a);
    }
    
    // Apply color override if enabled (for minimap)
    if (use_color_override == 1) {
        color = vec4(color_override, color.a);
    }
    
    // Assign color to fragment
    gl_FragColor = color;
    
    // Check for transparency
    if (color.a < 1.0) {
        discard;
    }
}