//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//
// #extension GL_EXT_gpu_shader4 : require

//precision mediump float; // required for IOS but not supported osx
// - or not - now added in Shader.cpp


//  Fragment shaders also have access to the discard command. When executed, this command causes the fragment's output values to be discarded.

//varying vec4 colorVarying;

varying vec2 TexCoordOut; // New

// lets get the orig texcoord out too....
varying vec2 OrigTexCoordOut; // New

//
//uniform float fishEyeScale;
//uniform float fishEyeScalePercent;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 ui_color;
uniform vec4 ui_foreground_color;
uniform vec4 ui_corner_radius;

//uniform vec4 ui_corner_radius_x_tl_tr_br_bl;
//uniform vec4 ui_corner_radius_y_tl_tr_br_bl;

const float fuzz = 0.025;
const float hfuzz =(fuzz * 0.5);


void roundCorner(vec2 OrigTexCoordOut, vec2 center, float radius, float fuzz) {

    float dis = distance(OrigTexCoordOut, center);
    if( dis >= radius ){
        gl_FragColor.a = gl_FragColor.a * (((radius + fuzz) - dis) / fuzz);
    }

}


void main()
{


//    if( !gl_FrontFacing ) {
//        gl_FragColor=vec4(255,0,0,255);
//    }

	vec4 ocolor=texture2D(texture1, TexCoordOut);


    if( ui_foreground_color.a > 0.0 ){

        if( ui_color.a > 0.0 ){


            gl_FragColor = mix(ui_color , ui_foreground_color * ocolor, ocolor.a);

            //gl_FragColor = mix(ui_color , ui_foreground_color, ocolor.a);

        }else{
            gl_FragColor = ui_foreground_color * ocolor;
        }

//        }else{
//            gl_FragColor = mix(ui_color , ui_foreground_color, ocolor.a);
//        }
    }else{

        if( ui_color.a > 0.0 ){
            gl_FragColor = ui_color;
            //
        }else{
            gl_FragColor = ui_foreground_color;
        }
    }
    /// IF YOU SEE ARTIFACTS - its because you are moving dragged item on sub-pixel increments
    // we should try to keep the level of drag matching full pixels but use hi-dpi pixels if available.


    float tl_radius = ui_corner_radius.r;
    float tr_radius = ui_corner_radius.g;
    float br_radius = ui_corner_radius.b;
    float bl_radius = ui_corner_radius.a;

    //gl_PointCoord
    //The location within a point primitive that defines the position of the fragment relative to the side of the point. Points are effectively rasterized as window-space squares of a certain pixel size. Since points are defined by a single vertex, the only way to tell where in that square a particular fragment is is with gl_PointCoord.
    //The values of gl_PointCoord's coordinates range from [0, 1]. OpenGL uses a upper-left origin for point-coordinates by default, so (0, 0) is the upper-left. However, the origin can be switched to a bottom-left origin by calling glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);




    if( tl_radius > 0.0 ) {
        float radius = tl_radius;
        //float rradius = 1.0 - radius;

        float fuzzradius = radius + hfuzz;
        //float rfuzzzradius = rradius - hfuzz;

        if( OrigTexCoordOut.y <= fuzzradius ){
            if( OrigTexCoordOut.x <= fuzzradius ) {
                // TOP LEFT
                roundCorner(OrigTexCoordOut, vec2(fuzzradius,fuzzradius), radius, fuzz);
            }
        }
    }

    if( tr_radius > 0.0 ) {
        float radius = tr_radius;
        float rradius = 1.0 - radius;

        float fuzzradius = radius + hfuzz;
        float rfuzzzradius = rradius - hfuzz;

        if( OrigTexCoordOut.y <= fuzzradius ){
            if(OrigTexCoordOut.x >= rfuzzzradius ) {
                // TOP RIGHT
                roundCorner(OrigTexCoordOut, vec2(rfuzzzradius ,fuzzradius), radius, fuzz);
            }
        }
    }

    if( br_radius > 0.0 ) {
        float radius = br_radius;
        float rradius = 1.0 - radius;

        //float fuzzradius = radius + hfuzz;
        float rfuzzzradius = rradius - hfuzz;

        if (OrigTexCoordOut.y >= rfuzzzradius){
            if(OrigTexCoordOut.x >= rfuzzzradius ) {
                // BOTTOM RIGHT
                roundCorner(OrigTexCoordOut, vec2(rfuzzzradius ,rfuzzzradius), radius, fuzz);
            }
        }
    }

    if( bl_radius > 0.0 ) {
        float radius = bl_radius;
        float rradius = 1.0 - radius;

        float fuzzradius = radius + hfuzz;
        float rfuzzzradius = rradius - hfuzz;

        if (OrigTexCoordOut.y >= rfuzzzradius){
            if( OrigTexCoordOut.x <= fuzzradius ) {
                // BOTTOM LEFT
                roundCorner(OrigTexCoordOut, vec2(fuzzradius,rfuzzzradius), radius, fuzz);

            }
        }
    }

//	if(ui_color.a > 0.0 ){ // background color
//
//        //gl_FragColor = mix(ui_foreground_color,  mix(ui_color, ocolor ,ocolor.a), ui_foreground_color.a );
//
//        vec4 firstMix = mix(ui_foreground_color, ocolor, ui_foreground_color.a * ocolor.a );
//
//
//        if( ui_foreground_color.a > 0.0 ){
//            gl_FragColor=firstMix;
//        }else{
//            gl_FragColor = mix(firstMix, ui_color, ocolor.a);
//        }
//
//    }else{
//        gl_FragColor = ocolor;
//	}
}

