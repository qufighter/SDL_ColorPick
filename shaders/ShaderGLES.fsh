//
//  Shader.fsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2019 Vidsbee All rights reserved.
//
// #extension GL_EXT_gpu_shader4 : require

//precision mediump float; // required for IOS but not supported osx
// ^^^ - now added automagically

varying vec4 colorVarying;
varying vec2 TexCoordOut; // New
varying vec2 backgroundTexCoord;


varying float texWidth;
varying float texWidthLessOne;
varying float halfTexWidth;


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

    //    float texWidth = 512.0;
    //    float halfTexWidth = 255.0;
    //    int halfTexWidthInt = 255;
    //    float halfTexWidthLessOne = 254.0;\

//    float texWidth = 2048.0; // 256.0;
//    float texWidthLessOne = 2047.0; // 256.0
//    float halfTexWidth = 1023.0; //127.0;
//    int halfTexWidthInt = 1023; // 127;
//    float halfTexWidthLessOne = 1022.0; // 126.0;

    //    float texWidth = 1024.0; //2048.0 512.0 256.0;
    //    float texWidthLessOne = 1023.0; //2047.0  511.0 255.0 // really doesnt need to be less1
    //    float halfTexWidth = 511.0; //1023.0 255.0 127.0; // does need to be less1
    //    int halfTexWidthInt = 511; //1023 255 127;

    // these computation really only need to occur once and are same for each pixel, does shader optmize this automatically?
    // we could move this into vertex shader -- it seems hardcoded values work better for mediump though...
//    float texWidth = textureWidth; //2048.0 512.0 256.0;
//    float texWidthLessOne = texWidth - 1.0; //2047.0  511.0 255.0 // really doesnt need to be less1
//    float halfTexWidth = (texWidth * 0.5) - 1.000000000; //1023.0 255.0 127.0; // does need to be less1
//    int halfTexWidthInt = int(halfTexWidth); //1023 255 127;


    // floor breaks ios... maybe we can hardcode it to 1023 anyway...
    int halfTexWidthInt = int(ceil(halfTexWidth)); // int cannot be output of vsh



    // todo: keep moving (non int) stuff to vsh???
    //float halfTexWidthLessOne = 1022.0; // 126.0;
    float reticuleSize = 0.1 + (0.5 * (1.0-fishEyeScalePercent));
    float retSize = 0.5 - (reticuleSize * 0.5);  // half of the size of the visibe pixel color reticule - the rest will be filled with ccolor
    float pointFour = retSize;
    float pointSix = 1.0 - retSize;




//    vec2 regTexCoord = TexCoordOut;// * vec2(1.0,1.3);
//    regTexCoord.y -= 0.15;
//    if( regTexCoord.y < 0.0 ){
//        regTexCoord.y = 0.0;
//    }else if( regTexCoord.y > 1.0 ){
//        regTexCoord.y = 1.0;
//    }


    // TODO we do not need ocolor until the end
//    vec4 ocolor;
//
//    if( backgroundTexCoord.x < 0.0 || backgroundTexCoord.x > 1.0 || backgroundTexCoord.y < 0.0 || backgroundTexCoord.y > 1.0 ){
//        ocolor=vec4(0.0,0.0,0.0,1.0);
//    }else{
//        ocolor=texture2D(texture2, backgroundTexCoord);
//    }
    //gl_FragColor = ocolor;

//    if(ocolor.a > 0.9){
//        gl_FragColor=ocolor;
//
//    }else{



//gl_FragColor = texture2D(texture1, TexCoordOut);
//return;
        vec2 octr=vec2(0.5,0.5);
        float dis=distance(octr,TexCoordOut) / fishEyeScale;

        float maxDis = (0.3 / (fishEyeScale));
//        if( fishEyeScale < 2.0 ){
//
//            maxDis=fishEyeScale*0.25;
//        }

    //pls try alws enable
        //if( dis > maxDis ){
            // blur if farther awy
            float scaler = (dis - maxDis) * (texWidth * 0.2);
            if( scaler < 1.0 ) scaler = 1.0;

            //float scaler = 400.0;


//            texWidth *= scaler; //Left-hand-side of assignment must not be read-only
//            texWidthLessOne *= scaler;
//            halfTexWidth *= scaler;


            //ccolor = vec4(1.0,1.0,1.0,1.0);
        //}

		vec2 inp=(TexCoordOut * texWidth);
		vec2 ctr=vec2(halfTexWidth,halfTexWidth);
		vec2 agl=(inp.xy - ctr.xy);
		//vec2 agl=(TexCoordOut.xy - octr.xy);
		//float dis=dist(ctr,inp);
		
		
	//correct code(such as floor(a+0.5).

        float aRetDist = 0.0001; // 0.0123 @ 255

		
    vec2 res=(ctr + (agl * dis)) ;//+ vec2(aRetDist,aRetDist);

        float halfPixel = 0.5;

		ivec2 get=ivec2(floor(res+halfPixel));

		//ivec2 ictr=ivec2(ctr);
			//if(inp_rough == res){
			//	gl_FragColor = vec4(255.0,255.0,255.0,255.0);
			//}else
				
		//vec4 bcolor=texelFetch(texture1, get, 0);

        vec2 pixelPosition = floor(res+halfPixel) / texWidthLessOne;

    if(  pixelPosition.x > 0.0 && pixelPosition.x < 1.0 && pixelPosition.y > 0.0 && pixelPosition.y < 1.0){

		vec4 bcolor=texture2D(texture1,  pixelPosition); //maths
        //vec4 ncolor=texture2D(texture1, (floor(res+halfPixel+0.01) / texWidthLessOne )); // next pxl


//gl_FragColor = bcolor;
//return;
		
		
		//vec4 pcolor = texelFetch2D(texture1, ivec2(ctr) , 0);//picked color
		vec4 pcolor=texture2D(texture1, ctr / texWidthLessOne );


        vec4 ccolor = vec4(0.0,0.0,0.0,1.0);//crosshair color

        if(pcolor.r + pcolor.g + pcolor.b < 1.5){
			ccolor = vec4(1.0,1.0,1.0,1.0);
		}// logical default
        //ccolor = vec4(1.0,1.0,1.0,2.0) - pcolor;//inverse
        //ccolor = (vec4(1.0,1.0,1.0,2.0) - pcolor) * 0.5;//inverse subdued

        //pointFour=0.2;

        if( (get.x == halfTexWidthInt && get.y == halfTexWidthInt) //){
           && (res.x < halfTexWidth - pointFour || res.x > halfTexWidth + pointFour || res.y < halfTexWidth - pointFour || res.y > halfTexWidth + pointFour) ){
			//if( res.x < halfTexWidthLessOne + pointSix || res.x > halfTexWidth + pointFour || res.y > halfTexWidth + pointFour || res.y < halfTexWidthLessOne + pointSix ){
            //    if( res.x > halfTexWidthLessOne + pointSix && res.x < halfTexWidth + pointFour && res.y < halfTexWidth + pointFour && res.y > halfTexWidthLessOne + pointSix ){

//                if( distance(ccolor, pcolor) < 0.015 ){// inverese isn't exact match grey
//                    ccolor += 0.3;
//                }

				bcolor = ccolor;
				//bcolor = mix(bcolor,ccolor,0.8);
				//bcolor = vec4(1.0,1.0,1.0,1.0);
				//
			//}
		}

        //circle
//        if( dis > 0.009 && dis < 0.01 ){
//            bcolor = ccolor;
//        }


//        if( ncolor != bcolor ){
////fxaa
//            bcolor = (ncolor + bcolor) * 0.5;
//            //bcolor= vec4(0.0,0.0,0.0,1.0);
//        }

		
	//corner color preview in shader helps determine if color is correct
//        float cornerSize = texWidth *0.45;
//        if( inp.x < cornerSize && inp.y > texWidth - cornerSize ){
//            //bcolor = texelFetch2D(texture1, ivec2(ctr) , 0);
//            bcolor = pcolor;
//        }

		
		//gl_FragColor = bcolor + (ocolor * ocolor.a);


        if( /*bcolor.r == 0.0 && bcolor.g == 0.0 && bcolor.b == 0.0 && */ bcolor.a == 0.0){

            //vec2 ctr=vec2(halfTexWidth,halfTexWidth);
            //vec2 agl=(inp.xy - ctr.xy);
            //vec2 agl=(TexCoordOut.xy - octr.xy);
//
//             inp=(TexCoordOut * vec2(2048.0, 1024.0));
//             ctr=vec2(halfTexWidth,halfTexWidth);
//             agl=(inp.xy - ctr.xy);


//            vec2 thisCoord = vec2(TexCoordOut);
//            thisCoord.x *= (1.333333333333333 * 0.5);
//            thisCoord.y *= 1.0;
//            dis=distance(octr,thisCoord) / fishEyeScale;

            //correct code(such as floor(a+0.5).

            //float aRetDist = 0.0001; // 0.0123 @ 255

//            res=(ctr + (agl * dis)) + vec2(aRetDist,aRetDist);
//
//            pixelPosition = floor(res+halfPixel) / texWidthLessOne;

            // test texture is really 1024 tall, we are 2048
            //pixelPosition.y *= 0.5;

            //pixelPosition.y *= 0.5;

            //bcolor=texture2D(texture2,  pixelPosition); //maths


            vec4 ocolor;
            if( backgroundTexCoord.x < 0.0 || backgroundTexCoord.x > 1.0 || backgroundTexCoord.y < 0.0 || backgroundTexCoord.y > 1.0 ){
                ocolor=vec4(0.0,0.0,0.0,1.0);
            }else{
                ocolor=texture2D(texture2, backgroundTexCoord);
            }
//            float desat = (ocolor.r + ocolor.g + ocolor.b) / 3.0;
//            ocolor.r=desat, ocolor.g=desat, ocolor.b=desat;
            //ocolor*=0.8;
            bcolor=ocolor;
        }
        gl_FragColor = bcolor;

    }else{


        vec4 ocolor;
        if( backgroundTexCoord.x < 0.0 || backgroundTexCoord.x > 1.0 || backgroundTexCoord.y < 0.0 || backgroundTexCoord.y > 1.0 ){
            ocolor=vec4(0.0,0.0,0.0,1.0);
        }else{
            ocolor=texture2D(texture2, backgroundTexCoord);
        }
//        float desat = (ocolor.r + ocolor.g + ocolor.b) / 3.0;
//        ocolor.r=desat, ocolor.g=desat, ocolor.b=desat;
        //ocolor*=0.8;

        //vec4 bcolor=texture2D(texture2,  pixelPosition); //maths
        //gl_FragColor=bcolor;
        gl_FragColor=ocolor;
    }
				//gl_FragColor = mix(bcolor,ocolor,ocolor.a);


		//gl_FragColor = bcolor;
		//if(ocolor.a > 0.5)
		//	gl_FragColor = ocolor;
	//}
}

