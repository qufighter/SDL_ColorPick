//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//


varying vec2 TexCoordOut; // New

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

void main()
{

	vec4 ocolor=texture2D(texture1, TexCoordOut);

    gl_FragColor = ocolor;

}

