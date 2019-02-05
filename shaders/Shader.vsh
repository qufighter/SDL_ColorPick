//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

attribute vec4 position;

uniform float widthHeightRatio;

uniform vec2 positionOffset;

uniform float textureWidth;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;

varying vec2 backgroundTexCoord;

varying float texWidth;
varying float texWidthLessOne;
varying float halfTexWidth;

void main()
{
	TexCoordOut = TexCoordIn;

    backgroundTexCoord = TexCoordOut + positionOffset;

    texWidth = textureWidth; //2048.0 512.0 256.0;
    texWidthLessOne = texWidth - 1.0; //2047.0  511.0 255.0 // really doesnt need to be less1
    halfTexWidth = (texWidth * 0.5) - 1.000000000; //1023.0 255.0 127.0; // does need to be less1

    if( widthHeightRatio > 1.0 ){ // widescreen
        gl_Position=vec4(position.x, position.y * widthHeightRatio, position.z, position.a);
    }else{
        gl_Position=vec4(position.x / widthHeightRatio, position.y, position.z, position.a);
    }
}
