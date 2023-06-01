#version 150

in vec3 pass_Normal, pass_Position, pass_Camera_Position;
in mat4 pass_ViewMatrix, pass_ModelMatrix, pass_NormalMatrix;

out vec4 out_Color;

uniform vec3 planet_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform float light_intensity;
uniform bool CellShadingMode;
uniform vec3 ambient_intensity;

float shine = 15.0f;
float specular_intensity = 0.5f;
float diffuse_intensity = 0.6f;
float outline = 1.0f;

void main() {
    vec3 normal = normalize(pass_Normal);
    vec3 vertex_pos = pass_Position;

    vec3 specular_color = vec3(1.0,1.0,1.0);

    float diffuse_reflection_factor = 0.9;
    float specular_reflection_factor = 0.9;
    int n = 32;

    vec3 camera_Position = pass_Camera_Position;

    vec3 transformed_light_position = (pass_ViewMatrix * vec4(light_position,1.0)).xyz;

    vec3 light_Direction = normalize(transformed_light_position - pass_Position);
    vec3 view_Direction = normalize(camera_Position - pass_Position);
    vec3 h = normalize(view_Direction + light_Direction);

    float diffuse_light_intensity = light_intensity * diffuse_reflection_factor * max(dot(normal,light_Direction),0);
    float specular_light_intensity = light_intensity * specular_reflection_factor * pow(max(dot(h,normal),0),n);

    //vec3 ambient = ambient_intensity * light_color;
    vec3 ambient = ambient_intensity;
    vec3 diffuse = diffuse_light_intensity * light_color;
    vec3 specular =  specular_light_intensity * specular_color;


    // Cellshading or Toon Shading
    if(!CellShadingMode) {
        out_Color = vec4((ambient + diffuse) * planet_color + specular, 1.0f);
    }
    else {
        if (diffuse_light_intensity > 0.95)
            diffuse = 1.0 * light_color;
        else if (diffuse_light_intensity > 0.6)
            diffuse = 0.7 * light_color;
        else if (diffuse_light_intensity > 0.2)
            diffuse = 0.35 * light_color;
        else
            diffuse = 0.1 * light_color;

        out_Color = vec4((ambient + diffuse) * planet_color, 1.0);
    }
}
