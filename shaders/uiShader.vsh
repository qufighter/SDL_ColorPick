//
//  Shader.vsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2012 Vidsbee All rights reserved.
//

attribute vec4 position;
//attribute vec4 color;

//varying vec4 colorVarying;

//uniform mat4 modelMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;

//uniform float translate;

//uniform mat4 modelMatrix;
//uniform mat4 projectionMatrix;
//uniform float widthHeightRatio;


attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;


uniform vec4 ui_scale;
uniform vec4 ui_position;
uniform vec4 texture_crop;


//uniform vec3 in_Position;

void main()
{
	//TexCoordOut = vec2((-texture_crop.r * texture_crop.b) + texture_crop.b * TexCoordIn.s,  (-texture_crop.g * texture_crop.a ) + texture_crop.a * TexCoordIn.t);


    float x=texture_crop.r;
    float y=texture_crop.g;
    float w = texture_crop.b;
    float h = texture_crop.a;

//    x=0.08333333333333*2.0;
//    y=0.3;
//    w=0.08333333333333;
//    h=0.1;

    float xscale = 1.0/w;
    float yscale = 1.0/h;

    TexCoordOut = vec2( (TexCoordIn.s * xscale) - (x * w),   (TexCoordIn.t * yscale) - (y * h));


    //TexCoordOut = TexCoordIn / 12.0;

    TexCoordOut = vec2( TexCoordIn.s / xscale + x,   TexCoordIn.t / yscale + y);

    //TexCoordOut = vec2( TexCoordIn.s / 12.0,   TexCoordIn.t / 10.0);

    //gl_Position = position;
   // gl_Position.x += sin(in_Position.x) / 2.0;
	//gl_Position.y += sin(in_Position.y) / 2.0;
	//gl_Position.z += sin(in_Position.z) / 2.0;
	
	//gl_Position=position + vec4(0.25,0.0,0.0,0.0);
    //gl_Position=position * projectionMatrix * modelMatrix;

    //gl_Position=(position + ui_position) * ui_scale;
    gl_Position=(position * ui_scale) + ui_position;

    //gl_Position=position * vec4(1.0,widthHeightRatio,1.0,1.0);

    //*viewMatrix;
	//gl_Position=position*(projectionMatrix*modelMatrix*viewMatrix);
    //colorVarying = color;
}
