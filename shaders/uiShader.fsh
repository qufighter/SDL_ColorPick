//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

varying vec2 TexCoordOut;
varying vec2 OrigTexCoordOut;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 ui_color;
uniform vec4 ui_foreground_color;
uniform vec4 ui_corner_radius;

const float fuzz = 0.0078125; // 0.025;
const float hfuzz =(fuzz * 0.5);

void roundCorner(vec2 OrigTexCoordOut, vec2 center, float radius, float fuzz) {
    float dis = distance(OrigTexCoordOut, center);
    if( dis >= radius ){
        gl_FragColor.a = gl_FragColor.a * (((radius + fuzz) - dis) / fuzz);
    }
}

void main()
{
	vec4 ocolor=texture2D(texture1, TexCoordOut);
    if( ui_foreground_color.a > 0.0 ){
        if( ui_color.a > 0.0 ){
            gl_FragColor = mix(ui_color , ui_foreground_color * ocolor, ocolor.a);
        }else{
            gl_FragColor = ui_foreground_color * ocolor;
        }
    }else{
        if( ui_color.a > 0.0 ){
            gl_FragColor = ui_color;
        }else{
            gl_FragColor = ui_foreground_color;
        }
    }

    float tl_radius = ui_corner_radius.r;
    float tr_radius = ui_corner_radius.g;
    float br_radius = ui_corner_radius.b;
    float bl_radius = ui_corner_radius.a;

    if( tl_radius > 0.0 ) {
        float radius = tl_radius;
        float fuzzradius = radius + hfuzz;

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
}
