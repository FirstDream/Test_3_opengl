uniform mat4 MVP;

in vec4 vVertices;
in vec2 vTexCoord;

out vec2 texcoord;

void main(void)
{
	texcoord = vTexCoord;
    gl_Position = MVP * vVertices;
}