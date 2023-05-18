#version 330 core
precision highp float;
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;
uniform float center_x;     // center of the mandelbrot set being rendered
uniform float center_y;
uniform float scale;        // scale aka zoom factor
uniform int iterations;     // maximum number of iterations
uniform float ratio;        // aspect ratio of screen

void main()
{
    vec2 c, z;
    c.x = (TexCoord.x - 0.5) * ratio * scale - center_x;            // center and scale x coord
    c.y = (TexCoord.y - 0.5) * scale - center_y;            // center and scale y coord

    int i;      // current iteration
    z = c;      // copy c to preserve it
    for (i = 0; i < iterations; ++i) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x * 2) + c.y;

        if ((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }
    FragColor = texture2D(tex, vec2((i == iterations ? 0.0 : float(i)) / 100.0), 1);
} 
