//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

varying vec4 colorVarying;
varying vec2 TexCoordOut;
varying vec2 backgroundTexCoord;

varying float texWidth;
varying float texWidthLessOne;
varying float halfTexWidth;

uniform float fishEyeScale;
uniform float fishEyeScalePercent;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

void main()
{
    // floor breaks ios... maybe we can hardcode it to 1023 anyway...
    int halfTexWidthInt = int(ceil(halfTexWidth)); // int cannot be output of vsh

    // todo: keep moving (non int) stuff to vsh???
    float reticuleSize = 0.1 + (0.5 * (1.0-fishEyeScalePercent));
    float retSize = 0.5 - (reticuleSize * 0.5);  // half of the size of the visibe pixel color reticule - the rest will be filled with ccolor
    float pointFour = retSize;
    float pointSix = 1.0 - retSize;

    vec2 octr=vec2(0.5,0.5);
    float dis=distance(octr,TexCoordOut) / fishEyeScale;

    float maxDis = (0.3 / (fishEyeScale));

        float scaler = (dis - maxDis) * (texWidth * 0.2);
        if( scaler < 1.0 ) scaler = 1.0;

    vec2 inp=(TexCoordOut * texWidth);
    vec2 ctr=vec2(halfTexWidth,halfTexWidth);
    vec2 agl=(inp.xy - ctr.xy);

    float aRetDist = 0.0001; // 0.0123 @ 255

    vec2 res=(ctr + (agl * dis)) ;

    float halfPixel = 0.5;

    ivec2 get=ivec2(floor(res+halfPixel));

    vec2 pixelPosition = floor(res+halfPixel) / texWidthLessOne;

    if(  pixelPosition.x > 0.0 && pixelPosition.x < 1.0 && pixelPosition.y > 0.0 && pixelPosition.y < 1.0){

		vec4 bcolor=texture2D(texture1,  pixelPosition); //maths

        vec4 pcolor=texture2D(texture1, ctr / texWidthLessOne );

        vec4 ccolor = vec4(0.0,0.0,0.0,1.0);//crosshair color

        if(pcolor.r + pcolor.g + pcolor.b < 1.5){
			ccolor = vec4(1.0,1.0,1.0,1.0);
		}

        if( (get.x == halfTexWidthInt && get.y == halfTexWidthInt) //){
           && (res.x < halfTexWidth - pointFour || res.x > halfTexWidth + pointFour || res.y < halfTexWidth - pointFour || res.y > halfTexWidth + pointFour) ){
				bcolor = ccolor;
		}

        //circle
//        if( dis > 0.009 && dis < 0.01 ){
//            bcolor = ccolor;
//        }

        //corner color preview
//        float cornerSize = texWidth *0.45;
//        if( inp.x < cornerSize && inp.y > texWidth - cornerSize ){
//            //bcolor = texelFetch2D(texture1, ivec2(ctr) , 0);
//            bcolor = pcolor;
//        }

        if( bcolor.a == 0.0){

            vec4 ocolor;
            if( backgroundTexCoord.x < 0.0 || backgroundTexCoord.x > 1.0 || backgroundTexCoord.y < 0.0 || backgroundTexCoord.y > 1.0 ){
                ocolor=vec4(0.0,0.0,0.0,1.0);
            }else{
                ocolor=texture2D(texture2, backgroundTexCoord);
            }

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
        gl_FragColor=ocolor;
    }
}
