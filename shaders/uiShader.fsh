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
//
//uniform float fishEyeScale;
//uniform float fishEyeScalePercent;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec4 ui_color;
uniform vec4 ui_foreground_color;

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

