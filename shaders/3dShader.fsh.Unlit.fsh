//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

varying vec2 TexCoordOut;
varying vec4 colorOut;
varying vec4 normalOut;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 color_additive;

void main()
{
    vec4 ocolor;
    ocolor =  colorOut;
    ocolor.rgb += color_additive.rgb;
    ocolor.a = min( color_additive.a, ocolor.a );
    gl_FragColor = ocolor;
}
