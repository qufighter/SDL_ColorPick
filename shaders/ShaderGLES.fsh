//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2012 Vidsbee All rights reserved.
//
// #extension GL_EXT_gpu_shader4 : require

precision mediump float; // required for IOS but not supported osx

varying vec4 colorVarying;

varying vec2 TexCoordOut; // New

uniform float fishEyeScale;
uniform float fishEyeScalePercent;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
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
    //gl_FragColor = texture2D(texture1, TexCoordOut);
	//gl_FragColor = colorVarying;
    //gl_FragColor = texture2D(texture1, TexCoordOut); // snap
    //gl_FragColor = texture2D(texture1, vec2(0.5,0.5)); // snap
//    gl_FragColor = texture2D(texture1, vec2(TexCoordOut.x,TexCoordOut.y)); // snap
    //return;


//    gl_FragColor = vec4(1.0,0.0,0.0,1.0);
//    gl_FragColor = vec4(0.0,TexCoordOut.x,TexCoordOut.y,1.0);
//    return;


    // these computation really only need to occur once and are same for each pixel, does shader optmize this automatically?
    float texWidth = 2048.0; // 256.0;
    float texWidthLessOne = 2047.0; // 256.0
    int halfTexWidthInt = 1023; // 127;
    float halfTexWidth = 1023.0; //127.0;
    float halfTexWidthLessOne = 1022.0; // 126.0;
    float reticuleSize = 0.1 + (0.5 * (1.0-fishEyeScalePercent));
    float retSize = 0.5 - (reticuleSize * 0.5);  // half of the size of the visibe pixel color reticule - the rest will be filled with ccolor
    float pointFour = retSize;
    float pointSix = 1.0 - retSize;

//    float texWidth = 512.0;
//    int halfTexWidthInt = 255;
//    float halfTexWidth = 255.0;
//    float halfTexWidthLessOne = 254.0;
//    float pointFour = 0.4;
//    float pointSix = 0.6;

    vec2 regTexCoord = TexCoordOut * vec2(1.0,1.3);
    regTexCoord.y -= 0.15;
    if( regTexCoord.y < 0.0 ){
        regTexCoord.y = 0.0;
    }else if( regTexCoord.y > 1.0 ){
        regTexCoord.y = 1.0;
    }

	vec4 ocolor=texture2D(texture2, regTexCoord);
	if(ocolor.a > 0.9){
		gl_FragColor=ocolor;

	}else{

//gl_FragColor = texture2D(texture1, TexCoordOut);
//return;

		vec2 inp=(TexCoordOut * texWidth);
		vec2 ctr=vec2(halfTexWidth,halfTexWidth);
		vec2 octr=vec2(0.5,0.5);
		vec2 agl=(inp.xy - ctr.xy);
		float dis=distance(octr,TexCoordOut) / fishEyeScale;
		//vec2 agl=(TexCoordOut.xy - octr.xy);
		//float dis=dist(ctr,inp);
		
		
	//correct code(such as floor(a+0.5).

        float aRetDist = 0.0001; // 0.0123 @ 255

		
		vec2 res=(ctr + (agl * dis)) + vec2(aRetDist,aRetDist);
		ivec2 get=ivec2(floor(res+0.5));
		//ivec2 ictr=ivec2(ctr);
			//if(inp_rough == res){
			//	gl_FragColor = vec4(255.0,255.0,255.0,255.0);
			//}else
				
		//vec4 bcolor=texelFetch(texture1, get, 0);
		vec4 bcolor=texture2D(texture1, floor(res+0.5) / texWidthLessOne );
		
//gl_FragColor = bcolor;
//return;
		
		
		//vec4 pcolor = texelFetch2D(texture1, ivec2(ctr) , 0);//picked color
		vec4 pcolor=texture2D(texture1, ctr / texWidthLessOne );
		vec4 ccolor = vec4(0.0,0.0,0.0,1.0);//crosshair color
		if(pcolor.r + pcolor.g + pcolor.b < 1.5){
			ccolor = vec4(1.0,1.0,1.0,1.0);
		}
		
		if( get.x == halfTexWidthInt && get.y == halfTexWidthInt ){
			if( res.x < halfTexWidthLessOne + pointSix || res.x > halfTexWidth + pointFour || res.y > halfTexWidth + pointFour || res.y < halfTexWidthLessOne + pointSix ){
				bcolor = ccolor;
				bcolor = mix(bcolor,ccolor,0.8);
				//bcolor = vec4(1.0,1.0,1.0,1.0);
				//
			}
		}
		
		
	//corner color preview in shader helps determine if color is correct
        float cornerSize = texWidth *0.25;
		if( inp.x < cornerSize && inp.y > texWidth - cornerSize ){
			//bcolor = texelFetch2D(texture1, ivec2(ctr) , 0);
			bcolor = pcolor;
		}

		
		//gl_FragColor = bcolor + (ocolor * ocolor.a);


		//gl_FragColor = bcolor;
		gl_FragColor = mix(bcolor,ocolor,ocolor.a);


		//gl_FragColor = bcolor;
		//if(ocolor.a > 0.5)
		//	gl_FragColor = ocolor;
	}
}

