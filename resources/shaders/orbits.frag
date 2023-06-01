#version 150
out vec4 out_orbitColor;

void main(){
    vec3 orbitColor = vec3(0.3, 0.45, 0.7);
    out_orbitColor = vec4(abs(normalize(orbitColor)), 1.0);
}
