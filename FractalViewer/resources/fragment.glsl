#version 460 core

in vec4 gl_FragCoord;
out vec4 FragColor;

uniform float time;
uniform float zoomLevel;
uniform vec2 pos;
uniform vec2 windowSize;

int max_iters = 2000;

vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	int iters = 0;

	double zoom = exp(zoomLevel);

	double cre = ((gl_FragCoord.x / windowSize.x) - 0.5) * windowSize.x / 320.0 / zoom + pos.x;// + (double(pos.x) / 320.0 / zoom);
	double cim = ((gl_FragCoord.y / windowSize.y) - 0.5) * windowSize.y / 320.0 / zoom + pos.y;// + (double(pos.y) / 320.0 / zoom);

	double re = 0.0;
	double im = 0.0;
	double re2 = 0.0;
	double im2 = 0.0;

	double q = (cre - 0.25) * (cre - 0.25) + cim * cim;
	bool cardoidCheck = (q * (q + (cre - 0.25)) <= (cim * cim) / 4.0);
	bool circleCheck = (((cre + 1.0) * (cre + 1.0) + cim * cim) <= 0.0625);
	if (cardoidCheck || circleCheck) {
		iters = max_iters;
	} else {
		while (re2 + im2 <= 4 && iters < max_iters) {
			im = 2 * re * im + cim;
			re = re2 - im2 + cre;
			re2 = re * re;
			im2 = im * im;
			iters++;
		}
	}

	if (iters == max_iters) {
		FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		im = 2 * re * im + cim;
		re = re2 - im2 + cre;
		re2 = re * re;
		im2 = im * im;
		iters++;

		float color = float(iters) + 1.0f - (log(log(sqrt(float(re2 + im2)))))/(log(2.0f));
		FragColor = vec4(hsv2rgb(vec3(color / 256.0, 1.0f, 1.0f)), 1.0f);
	}

	if (pow((gl_FragCoord.x / windowSize.x - 0.5) * windowSize.x, 2) + pow((gl_FragCoord.y / windowSize.y - 0.5) * windowSize.y, 2) <= 4) {
		FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}