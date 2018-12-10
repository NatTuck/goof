#version 300 es /* -*- mode: c -*- */

precision mediump float;
in  vec4 point_color;
out vec4 color;

void
main()
{
    float dist = abs(length(gl_PointCoord * 2.0 - 1.0));
    float delta = fwidth(dist);
    float weight = 1.0 - smoothstep(1.0 - delta, 1.0 + delta, dist);
    color = point_color;
    color.a = weight * color.a;
}

