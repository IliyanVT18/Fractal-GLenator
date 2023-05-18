#version 330 core
precision highp float;
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;
uniform vec2 center;        // center of the mandelbrot set being rendered
uniform float scale;        // scale aka zoom factor
uniform int iterations;     // maximum number of iterations
uniform float ratio;        // aspect ratio of screen
//uniform vec2 c;             // c parameter
//uniform vec2 p;             // p parameter

void main()
{
    vec2 p, c, z;
    c.x = 0.5667;
    c.y = 0.0;
    p.x = -0.5;
    p.y = 0.0;
    z.x = (TexCoord.x - 0.5) * ratio * scale - center.x;    // center and scale x coord
    z.y = (TexCoord.y - 0.5) * scale - center.y;            // center and scale y coord

    int i;      // current iteration
    vec2 z_prev;
    z_prev = z;
    for (i = 0; i < iterations; ++i) {
        if ((z.x * z.x + z.y * z.y) > 4.0) break;

        vec2 tmp = z;
        z.x = (z.x * z.x - z.y * z.y) + c.x + (p.x * z_prev.x - p.y * z_prev.y);
        z.y = (z.y * tmp.x * 2) + c.y + (p.x * z_prev.y + p.y + z_prev.x);
        z_prev = tmp;
    }
    FragColor = texture2D(tex, vec2((i == iterations ? 0.0 : float(i)) / 100.0), 1);
} 
