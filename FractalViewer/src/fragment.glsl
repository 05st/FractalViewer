#version 460 core

in vec4 gl_FragCoord;
out vec4 FragColor;

uniform float zoom;
uniform vec2 pos;

int max_iters = 200;

int width = 1280;
int height = 720;

void main() {
	int iterations = 0;

	double c_re = ((gl_FragCoord.x - width / 2.0) * 4.0 / width) / zoom + pos.x;
	double c_im = ((gl_FragCoord.y - height / 2.0) * 4.0 / width) / zoom + pos.y;

	double x = 0.0;
	double y = 0.0;

	double x2 = 0.0;
	double y2 = 0.0;

	while (x2 + y2 <= 4 && iterations < max_iters) {
		y = 2 * x * y + c_im;
		x = x2 - y2 + c_re;
		x2 = x * x;
		y2 = y * y;
		iterations++;
	}

	if (iterations == max_iters) {
		gl_FragDepth = 0.0f;
		FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		float color = float(iterations) / max_iters;
		gl_FragDepth = color;
		FragColor = vec4(color, 0.0f, 0.0f, 1.0f);
	}
	
	if (sqrt(pow(float(gl_FragCoord.x - width / 2.0), 2) + pow(float(gl_FragCoord.y - height / 2.0), 2)) <= 2) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}