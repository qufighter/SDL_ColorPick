//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

varying vec2 TexCoordOut;
varying vec4 colorOut;
varying vec4 normalOut;
varying vec4 unprojNormalOut;

varying vec3 PositionOut;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 color_additive;

uniform vec4 global_ambient;

uniform float fishEyeScale;
uniform float fishEyeScalePercent;

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

    float ratio = 1.00 / 1.52; // glass

    vec3 CamPos = vec3(0.0,0.0,0.0);

    vec3 I = normalize(PositionOut -CamPos);

    vec3 R = refract(I, normalize(unprojNormalOut.xyz), ratio);

    float scale = floor(fishEyeScalePercent * 128.0) * 2.0;

    if( scale > 1.0 ){ // EVEN NUMBERS only
        vec2 upScale = vec2(scale,scale);
        R.x *= 1.0 / upScale.x;
        R.y *= 1.0 / upScale.y;
        R.x += upScale.x * 0.5;
        R.y += upScale.y * 0.5;
    }

    vec4 reflection_color =texture2D(texture1, vec2( R.x, -R.y) + vec2(0.5,0.5) );

    gl_FragColor.rgb = mix(reflection_color.rgb,ocolor.rgb,0.125);

    gl_FragColor.a = ocolor.a;
}
