#version 300 es /* -*- mode: c -*- */

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

out vec4 point_color;

void
main()
{
    gl_PointSize = 40.0;
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    point_color = color;
}

