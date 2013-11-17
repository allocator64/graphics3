#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;

varying vec2 v_texcoord;

varying vec3 l;
varying vec3 v;
varying vec3 n;


//! [0]
void main()
{
	const vec4  diffColor = texture2D(texture, v_texcoord);
    const vec4  specColor = vec4 ( 0.7, 0.7, 0.0, 1.0 );
    const float specPower = 30.0;

    vec3 n2   = normalize ( n );
    vec3 l2   = normalize ( l );
    vec3 v2   = normalize ( v );
    vec3 r    = reflect ( -v2, n2 );
    vec4 diff = diffColor * max ( dot ( n2, l2 ), 0.0 );
    vec4 spec = specColor * pow ( max ( dot ( l2, r ), 0.0 ), specPower );

    gl_FragColor = diff + spec;

    // Set fragment color from texture
//    gl_FragColor = texture2D(texture, v_texcoord);
}
//! [0]

