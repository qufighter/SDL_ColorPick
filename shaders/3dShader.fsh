//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//


varying vec2 TexCoordOut; // New
varying vec4 colorOut;
varying vec4 normalOut;
//varying vec3 PositionOut;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 color_additive;

uniform vec4 global_ambient;

void main()
{

    vec3 globalLight = global_ambient.rgb;
    float ambient_light = global_ambient.a;

    // can we look at the depth?  gl_FragDepth

    vec4 ocolor; //=texture2D(texture1, TexCoordOut);
    //if(out_Color.a < 0.99)discard;

    ocolor =  colorOut;

    ocolor.rgb += color_additive.rgb;

    ocolor.a = min( color_additive.a, ocolor.a );

    float nNdotL = max(dot(globalLight,normalOut.xyz),ambient_light);

    ocolor.rgb *= nNdotL;

    gl_FragColor = ocolor;

    //gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);


//    gl_FragColor.a = gl_FragDepth;


    //gl_FragColor.a = 0.25;
}

