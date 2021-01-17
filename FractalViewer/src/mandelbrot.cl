__kernel int mandelbrotProcess(__global int iterations, int max_iters, int width, int height, int x, int y) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int iters = 0;
				
			double cre = (((double)x / (double)scr_width) - 0.5) * (double)scr_width / 320.0;
			double cim = (((double)y / (double)scr_height) - 0.5) * (double)scr_height / 320.0;

			double q = (cre - 0.25) * (cre - 0.25) + cim * cim;
			bool bulbcheck = (q * (q + (cre - 0.25)) <= (cim * cim) / 4);

			if (bulbcheck) {
				iters = max_iters;
			}
			else {
				double re = 0.0;
				double im = 0.0;
				double re2 = 0.0;
				double im2 = 0.0;

				while ((re2 + im2 <= 4.0) && (iters < max_iters)) {
					im = 2 * re * im + cim;
					re = re2 - im2 + cre;
					re2 = re * re;
					im2 = im * im;
					iters++;
				}
			}


		}
	}
}