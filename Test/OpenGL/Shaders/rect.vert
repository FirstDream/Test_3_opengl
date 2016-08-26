uniform mat4 MVP;

attribute vec4 vVertex;
attribute vec2 vTexCoord;

out vec2 texcoord;

void main(void)
{
	texcoord = vTexCoord;
    gl_Position = MVP * vVertex;
}