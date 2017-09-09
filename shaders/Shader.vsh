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

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform float widthHeightRatio;


attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;


//uniform vec3 in_Position;

void main()
{
	TexCoordOut = TexCoordIn;
    //gl_Position = position;
   // gl_Position.x += sin(in_Position.x) / 2.0;
	//gl_Position.y += sin(in_Position.y) / 2.0;
	//gl_Position.z += sin(in_Position.z) / 2.0;
	
	//gl_Position=position + vec4(0.25,0.0,0.0,0.0);
    gl_Position=position * projectionMatrix * modelMatrix;

    gl_Position=position;
    //gl_Position=position * vec4(1.0,widthHeightRatio,1.0,1.0);

    //*viewMatrix;
	//gl_Position=position*(projectionMatrix*modelMatrix*viewMatrix);
    //colorVarying = color;
}
