//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2012 Vidsbee All rights reserved.
//
// #extension GL_EXT_gpu_shader4 : require

//precision mediump float; // required for IOS but not supported osx
//SIGH - or not - now added in Shader.cpp


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
uniform float ui_corner_radius;

//uniform vec4 ui_corner_radius_x_tl_tr_br_bl;
//uniform vec4 ui_corner_radius_y_tl_tr_br_bl;


/*
float dist(vec2 a, vec2 b){


	float dis = sqrt((a.y - b.y) * 
                     (a.y - b.y)
					+(a.x - b.x) * 
                     (a.x - b.x)
		    );
	return dis;


//	vec2 result = a.xy - b.xy;
//	float result2 = dot(result, result);
//	float dis = inversesqrt(result2);
//	return dis;
}
*/
/*
void main()
{
	vec2 ctr=vec2(0.5,0.5);
	vec2 agl=(TexCoordOut.xy - ctr.xy);
	float dis=dist(ctr,TexCoordOut);
	//if(dis < 0.5 ){
		//gl_FragColor = texture2D(texture1, ctr);
		//gl_FragColor = texture2D(texture1, ctr + (agl * dis));
		gl_FragColor = texelFetch2D(texture1, ivec2((ctr + (agl * dis)) * 255.0) , 0);
	//}else {
	//	gl_FragColor = texture2D(texture1, TexCoordOut);
	//}

    
	//
}
*/

//
//vec2 roundvec2(vec2 in){
//	return floor(in + 0.5);
//}

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
    if( ui_corner_radius > 0.0 ) {
        float radius = ui_corner_radius;
        float rradius = 1.0 - radius;
        float fuzz = 0.025;
        float hfuzz =(fuzz * 0.5);

        float fuzzradius = radius + hfuzz;
        float rfuzzzradius = rradius - hfuzz;

        if( OrigTexCoordOut.y <= fuzzradius ){
            if( OrigTexCoordOut.x <= fuzzradius ) {
                // TOP LEFT
                roundCorner(OrigTexCoordOut, vec2(fuzzradius,fuzzradius), radius, fuzz);

            }else if(OrigTexCoordOut.x >= rfuzzzradius ) {
                // TOP RIGHT
                roundCorner(OrigTexCoordOut, vec2(rfuzzzradius ,fuzzradius), radius, fuzz);
            }
        }else if (OrigTexCoordOut.y >= rfuzzzradius){
            if( OrigTexCoordOut.x <= fuzzradius ) {
                // BOTTOM LEFT
                roundCorner(OrigTexCoordOut, vec2(fuzzradius,rfuzzzradius), radius, fuzz);

            }else if(OrigTexCoordOut.x >= rfuzzzradius ) {
                // BOTTOM RIGHT
                roundCorner(OrigTexCoordOut, vec2(rfuzzzradius ,rfuzzzradius), radius, fuzz);
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

