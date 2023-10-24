To compile, run the following command from the src directory:

g++ -std=c++11 main.cpp -o raytracer
To run the program and produce the output.ppm images, run the following command from the src directory once the code has compiled:

./raytracer

The quality and scene choices cause the run time to vary drastically. To run in a reasonable amount of time I suggest dropping the pixel_samples down to 30 or less, then dropping the image_width to about 300. Most scenes will render in less than 10 minutes in these conditions. The lower the quality the faster.

The result is a collection of ppms, that when opened as layers in GIMP then exported as a gif produce an animation that can be watched in a browser.
