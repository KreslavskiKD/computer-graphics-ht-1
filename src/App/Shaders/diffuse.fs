#version 330 core

in vec2 vert_pos;
out vec4 out_col;

uniform int iterations;
uniform float param1;
uniform float param2;
uniform float param3;

float julia(vec2 uv) {
	int j;
	for (int i = 0; i < iterations; i++){
		j++;
		vec2 c = vec2(param2 * 0.001, param3 * 0.001);
		vec2 d = vec2(param1* 0.001 * 0.005, 0.0);
		uv = vec2(uv.x  * uv.x - uv.y * uv.y, 2.0 * uv.x * uv.y) + c + d;
		if (length(uv) > float(iterations)) {
			break;
		}
	}
	return float(j)/float(iterations);
}

void main() {
    float f = julia(vert_pos);
	out_col = vec4(vec3(f), 1.0);
}