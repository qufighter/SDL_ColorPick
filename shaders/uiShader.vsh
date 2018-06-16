//
//  Shader.vsh
//
//  Created by Sam Larison on 4/16/12.
//  Copyright 2012 Vidsbee All rights reserved.
//

attribute vec4 position; // one of 4 sq verticies (-1,-1, 1,-1, 1,1, -1,1)
//attribute vec4 color;

//varying vec4 colorVarying;

//uniform mat4 modelMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;

//uniform float translate;

//uniform mat4 modelMatrix;
//uniform mat4 projectionMatrix;
//uniform float widthHeightRatio;


attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;
varying vec2 OrigTexCoordOut;



uniform vec4 ui_scale;
uniform vec4 ui_position;
uniform vec4 texture_crop;  // this means the texture position really.
uniform vec4 ui_crop;

//uniform vec3 in_Position;

void main()
{
	//TexCoordOut = vec2((-texture_crop.r * texture_crop.b) + texture_crop.b * TexCoordIn.s,  (-texture_crop.g * texture_crop.a ) + texture_crop.a * TexCoordIn.t);

    OrigTexCoordOut = TexCoordIn;


    float x=texture_crop.r;
    float y=texture_crop.g;
    float w = texture_crop.b;
    float h = texture_crop.a;

//    x=0.08333333333333*2.0;
//    y=0.3;
//    w=0.08333333333333;
//    h=0.1;

    float xscale = 1.0/w;
    float yscale = 1.0/h;

 //   TexCoordOut = vec2( (TexCoordIn.s * xscale) - (x * w),   (TexCoordIn.t * yscale) - (y * h));


    //TexCoordOut = TexCoordIn / 12.0;

    TexCoordOut = vec2( TexCoordIn.s / xscale + x,   TexCoordIn.t / yscale + y);

    //TexCoordOut = vec2( TexCoordIn.s / 12.0,   TexCoordIn.t / 10.0);

    //gl_Position = position;
   // gl_Position.x += sin(in_Position.x) / 2.0;
	//gl_Position.y += sin(in_Position.y) / 2.0;
	//gl_Position.z += sin(in_Position.z) / 2.0;
	
	//gl_Position=position + vec4(0.25,0.0,0.0,0.0);
    //gl_Position=position * projectionMatrix * modelMatrix;

    //gl_Position=(position + ui_position) * ui_scale;



//    vec4 tempPos = ui_position;
//
//    if( tempPos.y < ui_crop.y ){
//        tempPos.y = ui_crop.y ;
//    }
//
//    if( tempPos.y > ui_crop.y + ui_crop.a ){
//        tempPos.y = ui_crop.y + ui_crop.a ;
//    }

    //vec4 oPos = ui_position;

//    if( oPos.x < ui_crop.x){
//        oPos.x = ui_crop.x;
//    }
//    if( oPos.y < ui_crop.y){
//        oPos.y = ui_crop.y;
//    }

    //vec4 oPos =(position * ui_scale) + ui_position;

 // not always needed
    //vec4 parPos = (vec4(ui_crop.b,ui_crop.a,1.0,1.0)) + vec4(ui_crop.x,-ui_crop.y,0.0,0.0);
//    vec4 parPos = ui_crop;
//
//    if( oPos.y < parPos.y ){
//        oPos.y=parPos.y;
//    }

//    if( oPos.y > parPos.y + parPos.a ){
//        oPos.y=parPos.y + parPos.a;
//    }

    vec4 objPosition = (position * ui_scale) + ui_position;

    if( ui_crop.b != 0.0 || ui_crop.a != 0.0 ){

        //    float crop_positionx=ui_crop.r;
        //    float crop_positiony=ui_crop.g;
        //    float crop_scalex = ui_crop.b;
        //    float crop_scaley = ui_crop.a;
        vec4 crop_position=vec4(ui_crop.r, ui_crop.g, 0.0, 0.0);
        vec4 crop_scale=vec4(ui_crop.b, ui_crop.a, 1.0, 1.0);


        vec4 cropParentPosition = (position * crop_scale) + crop_position;


        if( position.y < 0.0 ){ // "bottom" left or right corner of sq

            float lostY = 0.0;

            if( objPosition.y < cropParentPosition.y ){
                lostY = cropParentPosition.y - objPosition.y;
                objPosition.y = cropParentPosition.y;
            }

            // now we move our texure coord, which is for btm right/left
            TexCoordOut.y -= (lostY / 2.0);

            // sorry but these units may be random and need calculation... we can compare ui_scale with crop_scale to possibly compute these values
            //^ the above likely only works because all fonts are scaled down

            OrigTexCoordOut.y -= (lostY * 4.0); // thiso only works for certain size elemtns

            /*there are more conditions to check here but tiling engine wraps and does not produce these edge conditions*/

        }else{

            float lostY = 0.0;

            // oob top coord
            if( objPosition.y > cropParentPosition.y ){
                lostY = objPosition.y - cropParentPosition.y;
                objPosition.y = cropParentPosition.y;
            }

            TexCoordOut.y += (lostY / 2.0);

            OrigTexCoordOut.y += (lostY * 4.0);


            // we need to check if we're outside the other side of the crop position rect..., so we move to the other corner (top of our rect went out bottom of crop rect)
            vec4 cropParentPositionOpposite = ((position * -1.0) * crop_scale) + crop_position;
            if( objPosition.y < cropParentPositionOpposite.y ){
                objPosition.y = cropParentPositionOpposite.y;
            }

        }

    }

    gl_Position=objPosition;
//
//    vec4 parent_scale = vec4(ui_crop.b, ui_crop.a, 1.0, 1.0);
//    vec4 cropPosition = ui_crop * parent_scale;
//
//    if( gl_Position.y < cropPosition.y ){
//        gl_Position.y = cropPosition.y;
//    }


    //gl_Position=position * vec4(1.0,widthHeightRatio,1.0,1.0);

    //*viewMatrix;
	//gl_Position=position*(projectionMatrix*modelMatrix*viewMatrix);
    //colorVarying = color;
}
