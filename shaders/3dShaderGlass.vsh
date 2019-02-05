//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;

varying vec4 colorOut;
varying vec4 normalOut;
varying vec4 unprojNormalOut;

varying vec3 PositionOut;

void main()
{
    colorOut = color;
    TexCoordOut = TexCoordIn;
    gl_Position=projectionMatrix * viewMatrix * modelMatrix * position;
    normalOut = projectionMatrix * modelMatrix * normalize(normal);
    unprojNormalOut = modelMatrix * normalize(normal);
    PositionOut = vec3(modelMatrix * position);
}
