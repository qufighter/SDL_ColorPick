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

uniform float widthHeightRatio;

uniform vec2 positionOffset;

uniform float textureWidth;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;

varying vec2 backgroundTexCoord;

varying float texWidth;
varying float texWidthLessOne;
varying float halfTexWidth;

//uniform vec3 in_Position;

void main()
{
	TexCoordOut = TexCoordIn;
    //gl_Position = position;
   // gl_Position.x += sin(in_Position.x) / 2.0;
	//gl_Position.y += sin(in_Position.y) / 2.0;
	//gl_Position.z += sin(in_Position.z) / 2.0;
	
	//gl_Position=position + vec4(0.25,0.0,0.0,0.0);
    //gl_Position=position * projectionMatrix * modelMatrix;

    backgroundTexCoord = TexCoordOut + positionOffset;

    //    // moved here from frag shader
    texWidth = textureWidth; //2048.0 512.0 256.0;
    texWidthLessOne = texWidth - 1.0; //2047.0  511.0 255.0 // really doesnt need to be less1
    halfTexWidth = (texWidth * 0.5) - 1.000000000; //1023.0 255.0 127.0; // does need to be less1


    if( widthHeightRatio > 1.0 ){ //squareify
        // widescreen
        gl_Position=vec4(position.x, position.y * widthHeightRatio, position.z, position.a);
    }else{
        gl_Position=vec4(position.x / widthHeightRatio, position.y, position.z, position.a);
    }



    //gl_Position=position;
    //gl_Position=position * vec4(1.0,widthHeightRatio,1.0,1.0);

    //*viewMatrix;
	//gl_Position=position*(projectionMatrix*modelMatrix*viewMatrix);
    //colorVarying = color;
}
