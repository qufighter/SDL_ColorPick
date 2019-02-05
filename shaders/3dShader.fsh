//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

varying vec2 TexCoordOut;
varying vec4 colorOut;
varying vec4 normalOut;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 color_additive;

uniform vec4 global_ambient;

void main()
{
    vec3 globalLight = global_ambient.rgb;
    float ambient_light = global_ambient.a;
    vec4 ocolor;
    ocolor =  colorOut;
    ocolor.rgb += color_additive.rgb;
    ocolor.a = min( color_additive.a, ocolor.a );
    float nNdotL = max(dot(globalLight,normalOut.xyz),ambient_light);
    ocolor.rgb *= nNdotL;
    gl_FragColor = ocolor;
}
