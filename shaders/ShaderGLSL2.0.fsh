//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2012 Vidsbee All rights reserved.
//
// #extension GL_EXT_gpu_shader4 : require

varying vec4 colorVarying;

varying vec2 TexCoordOut; // New

uniform float fishEyeScale;

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
    //return;

    float texWidth = 256.0;
    int halfTexWidthInt = 127;
    float halfTexWidth = 127.0;
    float halfTexWidthLessOne = 126.0;
    float pointFour = 0.4;
    float pointSix = 0.6;

//    float texWidth = 512.0;
//    int halfTexWidthInt = 255;
//    float halfTexWidth = 255.0;
//    float halfTexWidthLessOne = 254.0;
//    float pointFour = 0.4;
//    float pointSix = 0.6;

	vec4 ocolor=texture2D(texture2, TexCoordOut);
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
		
		
		vec2 res=(ctr + (agl * dis)) + vec2(0.0123,0.0123);
		ivec2 get=ivec2(floor(res+0.5));
		//ivec2 ictr=ivec2(ctr);
			//if(inp_rough == res){
			//	gl_FragColor = vec4(255.0,255.0,255.0,255.0);
			//}else
				
		//vec4 bcolor=texelFetch(texture1, get, 0);
		vec4 bcolor=texture2D(texture1, floor(res+0.5) / 255.0);
		
//gl_FragColor = bcolor;
//return;
		
		
		//vec4 pcolor = texelFetch2D(texture1, ivec2(ctr) , 0);//picked color
		vec4 pcolor=texture2D(texture1, ctr / 255.0 );
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
		
		
	//corner color preview in shader	
//		if( inp.x < 64.0 && inp.y > 192.0 ){
//			//bcolor = texelFetch2D(texture1, ivec2(ctr) , 0);
//			bcolor = pcolor;
//		}
		
		
		//gl_FragColor = bcolor + (ocolor * ocolor.a);


		//gl_FragColor = bcolor;
		gl_FragColor = mix(bcolor,ocolor,ocolor.a);

		//gl_FragColor = bcolor;
		//if(ocolor.a > 0.5)
		//	gl_FragColor = ocolor;
	}
}

