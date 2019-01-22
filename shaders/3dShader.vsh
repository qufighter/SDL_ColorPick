//
//  Shader.vsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//

attribute vec4 position; // one of 4 sq verticies (-1,-1, 1,-1, 1,1, -1,1)
//attribute vec4 color;

//varying vec4 colorVarying;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;

void main()
{
    TexCoordOut = TexCoordIn;
	//gl_Position=position*(modelMatrix*(projectionMatrix*viewMatrix));
    gl_Position=projectionMatrix * viewMatrix * modelMatrix * position;
}
