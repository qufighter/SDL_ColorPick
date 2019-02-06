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

bool oobCheck(vec2 c){
    return c.x < 0.0 || c.x > 1.0 || c.y < 0.0 || c.y > 1.0;
}

void main()
{
    // floor breaks ios... maybe we can hardcode it to 1023 anyway...
    int halfTexWidthInt = int(ceil(halfTexWidth)); // int cannot be output of vsh

    // todo: keep moving (non int) stuff to vsh???
    float reticuleSize = 0.1 + (0.5 * (1.0-fishEyeScalePercent));
    float retSize = 0.5 - (reticuleSize * 0.5);  // half of the size of the visible pixel color reticule - the rest will be filled with ccolor

    vec2 octr=vec2(0.5,0.5);
    float dis=distance(octr,TexCoordOut) / fishEyeScale;

    vec2 inp=(TexCoordOut * texWidth);
    vec2 ctr=vec2(halfTexWidth,halfTexWidth);
    vec2 agl=(inp.xy - ctr.xy);

    float aRetDist = 0.0001; // 0.0123 @ 255

    vec2 res=(ctr + (agl * dis));

    float halfPixel = 0.5;

    ivec2 get=ivec2(floor(res+halfPixel));

    vec2 pixelPosition = floor(res+halfPixel) / texWidthLessOne;

    vec4 ocolor;

    if( pixelPosition.x > 0.0 && pixelPosition.x < 1.0 && pixelPosition.y > 0.0 && pixelPosition.y < 1.0){

		vec4 bcolor=texture2D(texture1,  pixelPosition); //maths

        vec4 pcolor=texture2D(texture1, ctr / texWidthLessOne );

        vec4 ccolor = vec4(0.0,0.0,0.0,1.0);//crosshair color

        if(pcolor.r + pcolor.g + pcolor.b < 1.5){
			ccolor = vec4(1.0,1.0,1.0,1.0);
		}

        if( (get.x == halfTexWidthInt && get.y == halfTexWidthInt)
           && (res.x < halfTexWidth - retSize || res.x > halfTexWidth + retSize || res.y < halfTexWidth - retSize || res.y > halfTexWidth + retSize) ){
                bcolor = ccolor;
        }

        // outline where ever the selected color also is... 0.1 needs to be scaled based on distance
//        if( pcolor.rgb == bcolor.rgb ){
//            vec2 TlEdgePixelPosition = floor(res+halfPixel-0.1) / texWidthLessOne;
//            vec4 tlEdgeColor=texture2D(texture1,  TlEdgePixelPosition);
//            if( tlEdgeColor.rgb != bcolor.rgb ){
//                bcolor=ccolor;
//            }else{
//                vec2 TrEdgePixelPosition = floor(res+halfPixel+0.1) / texWidthLessOne;
//                vec4 trEdgeColor=texture2D(texture1,  TrEdgePixelPosition);
//                if( trEdgeColor.rgb != bcolor.rgb ){
//                    bcolor=ccolor;
//                }
//            }
//        }

        //circle
//        if( dis > 0.009 && dis < 0.01 ){
//            bcolor = ccolor;
//        }

        //corner color preview
//        float cornerSize = texWidth *0.45;
//        if( inp.x < cornerSize && inp.y > texWidth - cornerSize ){
//            bcolor = pcolor;
//        }

        if( bcolor.a == 0.0){
            if( oobCheck(backgroundTexCoord) ){
                ocolor=vec4(0.0,0.0,0.0,1.0);
            }else{
                ocolor=texture2D(texture2, backgroundTexCoord);
            }

            bcolor=ocolor;
        }
        gl_FragColor = bcolor;

    }else{
        if( oobCheck(backgroundTexCoord) ){
            ocolor=vec4(0.0,0.0,0.0,1.0);
        }else{
            ocolor=texture2D(texture2, backgroundTexCoord);
        }
        gl_FragColor=ocolor;
    }
}
