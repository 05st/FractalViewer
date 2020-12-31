#version 460 core

in vec4 gl_FragCoord;
out vec4 FragColor;

uniform float time;
uniform float zoom;
uniform vec2 pos;
uniform vec2 windowSize;
uniform bool mandelbrotMode;
uniform vec2 j_c;
uniform int max_iters;
uniform bool cursor;
uniform float scale;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	int iterations = 0;

	float c_re = ((((gl_FragCoord.x) / windowSize.x) - 0.5) * windowSize.x) / (scale * zoom);
	float c_im = ((((gl_FragCoord.y) / windowSize.y) - 0.5) * windowSize.y) / (scale * zoom);

	c_re += ((((pos.x) / windowSize.x)) * windowSize.x) / scale;
	c_im += ((((pos.y) / windowSize.y)) * windowSize.y) / scale;

	if (mandelbrotMode) {
		// Mandelbrot Set
		float re = 0.0;
		float im = 0.0;

		float re2 = 0.0;
		float im2 = 0.0;

		while (re2 + im2 <= 4 && iterations < max_iters) {
			im = 2 * re * im + c_im;
			re = re2 - im2 + c_re;
			re2 = re * re;
			im2 = im * im;
			iterations++;
		}
	} else {
		// Julia Sets
		float re = c_re;
		float im = c_im;

		while (re*re + im*im <= 4 && iterations < max_iters) {
			float temp_re = re * re - im * im + j_c.x;
			im = 2 * im * re + j_c.y;
			re = temp_re;
			iterations++;
		}
	}

	if (iterations == max_iters) {
		gl_FragDepth = 0.0f;
		FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		// float color = float(iterations);
		float color1 = sqrt(float(iterations)) / sqrt(max_iters);
		float color2 = sqrt(float(iterations+10)) / sqrt(max_iters);
		float color = mix(color1, color2, mod(sqrt(float(iterations)) / sqrt(max_iters), 1));
		// float color = float(iterations) / max_iters;
		FragColor = vec4(hsv2rgb(vec3(color, 1.0f, 1.0f - color)).xyz, 1.0f);
	}

	if (cursor) {
		if (sqrt(pow(float(gl_FragCoord.x - windowSize.x / 2.0), 2) + pow(float(gl_FragCoord.y - windowSize.y / 2.0), 2)) <= 2) {

			FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}
