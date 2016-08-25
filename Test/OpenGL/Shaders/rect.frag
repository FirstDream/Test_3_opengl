uniform sampler2D iTexture;

out vec4 color;

in vec2 texcoord;

void main(void)
{
    //color = texture2D( iTexture, texcoord );
	color = vec4(1.0, 0.0, 0.0, 1.0 );
}