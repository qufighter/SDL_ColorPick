//
//  Shader.vsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//

attribute vec4 position; // one of 4 sq verticies (-1,-1, 1,-1, 1,1, -1,1)
attribute vec4 color;
attribute vec4 normal;

//varying vec4 colorVarying;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;

varying vec4 colorOut;
varying vec4 normalOut;
//varying vec3 PositionOut;


void main()
{
    colorOut = color;
    TexCoordOut = TexCoordIn;



	//gl_Position=position*(modelMatrix*(projectionMatrix*viewMatrix));
    //gl_Position=viewMatrix * projectionMatrix * modelMatrix * position;

    gl_Position=projectionMatrix * viewMatrix * modelMatrix * position;

    normalOut = projectionMatrix * modelMatrix * normalize(normal);


    //PositionOut = vec3(modelMatrix * position);

    //gl_FragDepth = gl_Position.z; // really the fsh should set this if any...
}
