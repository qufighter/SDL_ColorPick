//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//


varying vec2 TexCoordOut; // New
varying vec4 colorOut;
varying vec4 normalOut;
varying vec4 unprojNormalOut;

varying vec3 PositionOut;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 color_additive;

void main()
{

    float globalLightAmt = 0.3;
    vec3 globalLight = vec3(globalLightAmt,globalLightAmt,globalLightAmt);
    float ambient_light = 0.005;

    // can we look at the depth?  gl_FragDepth

    vec4 ocolor; //=texture2D(texture1, TexCoordOut);
    //if(out_Color.a < 0.99)discard;

    ocolor =  colorOut;

    ocolor.rgb += color_additive.rgb;

    ocolor.a = min( color_additive.a, ocolor.a );

    float nNdotL = max(dot(globalLight,normalOut.xyz),ambient_light);

    ocolor.rgb *= nNdotL;

/*

 In the fragment language, built-in variables are intrinsically declared as:
 in  vec4  gl_FragCoord;
 in  bool  gl_FrontFacing;
 in  float gl_ClipDistance[];
 in  vec2  gl_PointCoord;
 in  int   gl_PrimitiveID;
 in  int   gl_SampleID;
 in  vec2  gl_SamplePosition;
 out vec4  gl_FragColor;
 out vec4  gl_FragData[gl_MaxDrawBuffers];  // deprecated
 out float gl_FragDepth;
 out int   gl_SampleMask[];
 */

    float screenw = 320.0;
    float screenh = 480.0;
    float screena = screenw / screenh; // viewport_ratio

    float rem = (1.0 - screena) * 0.5;

    vec2 screenSpaceCoord = vec2((gl_FragCoord.x / screenw),( - gl_FragCoord.y / screenh) );

    //we will try to zoom into the center of the texture...
    screenSpaceCoord.x = (screenSpaceCoord.x * 0.5)  + 0.25;
    screenSpaceCoord.y = (screenSpaceCoord.y * 0.5)  - 0.25;

    // we will fix the aspect ratio....
    screenSpaceCoord.x = (screenSpaceCoord.x * screena) + rem;


    // gl_FragCoord - screen space coord xyz
//    ocolor =texture2D(texture1, screenSpaceCoord);
//     ocolor.a=1.0;

    float ratio = 1.00 / 1.52; // glass
////    vec3 I = normalize(Position - cameraPos);
//    //vec3 I = normalize( vec3(( -gl_FragCoord.x / screenw),( -gl_FragCoord.y / screenh), 0.0) );
//    //vec3 I = (PositionOut) * 0.5;
    //vec3 I = normalize(vec3(screenSpaceCoord.x, screenSpaceCoord.y, 0.0));

// TODO fix camPos ???

    vec3 CamPos = vec3(0.0,0.0,PositionOut.z-1.0);

    //vec3 I = normalize(vec3(-PositionOut.x, -PositionOut.y, PositionOut.z) -CamPos);
    vec3 I = normalize(PositionOut -CamPos);



    vec3 R = refract(I, normalize(unprojNormalOut.xyz), ratio);


    //vec3 R = reflect(I, normalize(normalOut.xyz));
    // TODO: we center our texture access here, but we also need to scale it appropriately somehow...... (cam pos?)
    /// fish scale thats missing... so maybe the texture is just wrong....


    /// R is for a cube map, which means the x coordinate is *4
    //R.x *= 2.0;
    //R.x += 2.0;

    //R.x *= 2.0;
    //R.y *= 0.5;

    // seems to only work for EVEN NUMBERS
    //vec2 upScale = vec2(64.0,64.0);
    vec2 upScale = vec2(64.0,64.0);
    //vec2 upScale = vec2(48.0,48.0);

    R.x *= 1.0 / upScale.x;
    R.y *= 1.0 / upScale.y;

    R.x += upScale.x * 0.5;
    R.y += upScale.y * 0.5;

    

    // seems to only work for x^2 series
//    float scaleItUp = 128.0;
//    scaleItUp = 2.0;

    // since our pick image has lots of blank space... lets scale this texture coord to meet our ROI
//    R *= 0.5;
//    R += vec3(1.0,1.0,1.0);
//
//    R *= 0.25;
//    R += vec3(2.0,2.0,2.0);
//

//    R *= 0.125;
//    R += vec3(4.0,4.0,4.0);

//    R *= 1.0 / scaleItUp;
//    R.xy += vec2(scaleItUp * 0.5);

//    if( R.z > 0.0 ){
//        R.x *= -1.0;
//    }

    // I think the artifacts are because hte normals are face normals, which is sort of annoying....
    vec4 reflection_color =texture2D(texture1, vec2( R.x, -R.y) + vec2(0.5,0.5) );



   // vec4 reflection_color =texture3D(texture1, R );

    //ocolor.a = 1.0; // refraction IS our transparancy right?

    //vec4 reflection_color =texture2D(texture1, vec2(I.x, -I.y) + vec2(0.5,0.5));


    gl_FragColor.rgb = reflection_color.rgb;


    //gl_FragColor.rgb = mix(reflection_color.rgb,ocolor.rgb,0.125);

    gl_FragColor.a = ocolor.a;



    // we do need blur though!?

    //gl_FragColor = ocolor;

    //gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);


//    gl_FragColor.a = gl_FragDepth;


    //gl_FragColor.a = 0.25;
}

