uniform sampler2D iTexture;
uniform vec2 vWindowColor;

out vec4 color;

in vec2 texcoord;

void main(void)
{
    vec4 v = texture2D( iTexture, texcoord );

	v = 0.5 +  ( v - vWindowColor.x ) / vWindowColor.y;

	color = vec4( v.xyz, 1.0 );
}