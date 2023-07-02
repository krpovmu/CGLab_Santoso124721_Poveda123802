#version 150

in vec2 pass_TexCoord;

out vec4 out_Color;

uniform sampler2D screen_Texture;

uniform vec2 texture_Size;

uniform bool horizontal_Mirroring;
uniform bool vertical_Mirroring;
uniform bool greyscale_mode;
uniform bool blur_mode;

vec2 tex_coords = pass_TexCoord;

void main() {

    if (horizontal_Mirroring) {
        tex_coords.y = 1.0 - tex_coords.y;
    }

    if (vertical_Mirroring) {
        tex_coords.x = 1.0 - tex_coords.x;
    }

    if (blur_mode) {

        vec2 pixel_Size = vec2(1.0, 1.0) / texture_Size;

        // neighbours
        vec2 neighbours[9] = vec2[](
            vec2(-pixel_Size.x, pixel_Size.y), vec2(0.0f, pixel_Size.y), vec2(pixel_Size.x, pixel_Size.y),
            vec2(-pixel_Size.x, 0.0f), vec2(0.0f, 0.0f), vec2(pixel_Size.x, 0.0f),
            vec2(-pixel_Size.x, -pixel_Size.y), vec2(0.0f, -pixel_Size.y), vec2(pixel_Size.x, -pixel_Size.y)
        );

        // weighting
        float gaussian_kernel[9] = float[](
            1.0/16, 1.0/8, 1.0/16,
            1.0/8, 1.0/4, 1.0/8,
            1.0/16, 1.0/8, 1.0/16
        );

        // weighted sum of the colors of the neighbouring pixels
        vec3 color = vec3(0.0);
        for (int i = 0; i<9; i++) {
            color += vec3(texture(screen_Texture, tex_coords.xy + neighbours[i])) * gaussian_kernel[i];
        }
        out_Color = vec4(color, 1.0);
    }
    else
    {
        out_Color = texture(screen_Texture, tex_coords);
    }

    if (greyscale_mode) {
        float luminance = (0.2126 * out_Color.r+0.7152 * out_Color.g+0.0722 * out_Color.b);
        out_Color = vec4(luminance, luminance, luminance, 1.0);
    }
}