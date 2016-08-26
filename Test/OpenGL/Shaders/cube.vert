uniform mat4 MVP;
uniform vec4 Color;

attribute vec3 vertex;
attribute vec3 normal;
attribute vec4 color;
void main()
{
    gl_FrontColor = color*Color;
    gl_Position = MVP*vec4(vertex,1.0);
}
