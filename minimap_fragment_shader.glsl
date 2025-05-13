#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;

// Color override for minimap dots
uniform vec3 color_override;
uniform int use_color_override;

void main()
{
    if (use_color_override == 1) {
        // Simple solid color with soft circle shape
        vec2 center = vec2(0.5, 0.5);
        float dist = distance(uv_interp, center);
        float alpha = smoothstep(0.5, 0.3, dist);
        gl_FragColor = vec4(color_override, alpha);
    } else {
        // Default to texture
        gl_FragColor = texture2D(onetex, uv_interp);
    }
}