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

vec2 resolution = vec2(1024, 768);

void main() {

    if (horizontal_Mirroring) {
        tex_coords.y = 1.0 - tex_coords.y;
    }

    if (vertical_Mirroring) {
        tex_coords.x = 1.0 - tex_coords.x;
    }

    if (blur_mode) {
        //newnew
        vec2 uv = tex_coords;
        vec4 color = vec4(0.0);

        const mat3 gaussian_kernel = mat3(
            1. / 16., 2. / 16., 1. / 16.,
            2. / 16., 4. / 16., 2. / 16.,
            1. / 16., 2. / 16., 1. / 16.
            );

        float direction[3] = float[](-1.0, 0.0, 1.0);
        
        for(int x_dir = 0; x_dir < 3; ++x_dir) {
            for (int y_dir = 0; y_dir < 3; ++y_dir) {
                vec2 movement = vec2(direction[x_dir], direction[y_dir]) / resolution.xy;
                
                color += texture(screen_Texture, uv + movement) * gaussian_kernel[x_dir][y_dir];
            }
        }

        out_Color = color;
        //newnew

        //oldold --> wird output, like nothing happend, idk 
        //vec2 pixel_Size = vec2(1.0, 1.0) / texture_Size;
        // neighbours
        //vec2 neighbours[9] = vec2[](
        //    vec2(-pixel_Size.x, pixel_Size.y), vec2(0.0f, pixel_Size.y), vec2(pixel_Size.x, pixel_Size.y),
        //    vec2(-pixel_Size.x, 0.0f), vec2(0.0f, 0.0f), vec2(pixel_Size.x, 0.0f),
        //    vec2(-pixel_Size.x, -pixel_Size.y), vec2(0.0f, -pixel_Size.y), vec2(pixel_Size.x, -pixel_Size.y)
        //);
        // weighting
        //float gaussian_kernel[9] = float[](
        //    1.0/16, 1.0/8, 1.0/16,
        //    1.0/8, 1.0/4, 1.0/8,
        //    1.0/16, 1.0/8, 1.0/16
        //);
        // weighted sum of the colors of the neighbouring pixels
        //vec3 color = vec3(0.0);
        //for (int i = 0; i<9; i++) {
        //    color += vec3(texture(screen_Texture, tex_coords.xy + neighbours[i])) * gaussian_kernel[i];
        //}
        //out_Color = vec4(color, 1.0);
        //oldold
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