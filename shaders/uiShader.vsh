//  Copyright 2019 Sam Larison and Vidsbee.com All rights reserved.

attribute vec4 position; // one of 4 sq verticies (-1,-1, 1,-1, 1,1, -1,1)

uniform mat4 modelMatrix;

attribute vec2 TexCoordIn;
varying vec2 TexCoordOut;
varying vec2 OrigTexCoordOut;

uniform vec4 ui_scale;
uniform vec4 ui_position;
uniform vec4 texture_crop;  // texture position
uniform vec4 ui_crop;
uniform vec4 ui_crop2;

void main()
{
    OrigTexCoordOut = TexCoordIn;

    float ui_texture_x=texture_crop.r;
    float ui_texture_y=texture_crop.g;

    float ui_texture_xscale = texture_crop.b;
    float ui_texture_yscale = texture_crop.a;

    float ui_texture_xscaler = 1.0/ui_texture_xscale;
    float ui_texture_yscaler = 1.0/ui_texture_yscale;

    vec4 effectiveScale = modelMatrix * ui_scale;
    vec4 objPosition = (modelMatrix * position * ui_scale) + ui_position;

    if( ui_crop.b != 0.0 || ui_crop.a != 0.0 ){

        vec4 crop_position=vec4(ui_crop.r, ui_crop.g, 0.0, 0.0);
        vec4 crop_scale=vec4(ui_crop.b, ui_crop.a, 1.0, 1.0);

        vec4 cropParentPosition = (position * crop_scale) + crop_position;

        bool hasSecondCropParent = false;
        vec4 cropParentPosition2;
        if( ui_crop2.b != 0.0 || ui_crop2.a != 0.0 ){
            vec4 crop_position2=vec4(ui_crop2.r, ui_crop2.g, 0.0, 0.0);
            vec4 crop_scale2=vec4(ui_crop2.b, ui_crop2.a, 1.0, 1.0);
            cropParentPosition2 = (position * crop_scale2) + crop_position2;
            hasSecondCropParent = true;
        }

        if( position.y < 0.0 ){ // "bottom" left or right corner of sq (unless rotated)

            float lostY = 0.0;
            float lostX = 0.0;

            if( hasSecondCropParent && cropParentPosition.y < cropParentPosition2.y ){
                cropParentPosition.y = cropParentPosition2.y;
            }

            if( objPosition.y < cropParentPosition.y ){
                lostY = cropParentPosition.y - objPosition.y;
                objPosition.y = cropParentPosition.y;
            }

            if( position.x  < 0.0 ){ // left

                if( objPosition.x < cropParentPosition.x ){
                    lostX = cropParentPosition.x - objPosition.x;
                    objPosition.x = cropParentPosition.x;
                }

            }else{ // right

                if( objPosition.x > cropParentPosition.x ){
                    lostX = objPosition.x - cropParentPosition.x;
                    objPosition.x = cropParentPosition.x;
                }

                //objPosition.x -= 0.1;
            }

            // now we move our texure coord, which is for btm right/left
            ui_texture_y -= ui_texture_yscale * (lostY / effectiveScale.y) * 0.5;

            // this modifies rounding corners somehow? needs work - only works for certain size elemtns fixme
            OrigTexCoordOut.y -= (lostY * 4.0);

            /*there are more conditions to check here but tiling engine wraps and does not produce these edge conditions*/

        }else{ // "top" left or right corner (unless rotated)

            float lostY = 0.0;
            float lostX = 0.0;

            if( hasSecondCropParent && cropParentPosition.y > cropParentPosition2.y ){
                cropParentPosition.y = cropParentPosition2.y;
            }

            // oob top coord
            if( objPosition.y > cropParentPosition.y ){
                lostY = objPosition.y - cropParentPosition.y;
                objPosition.y = cropParentPosition.y;
            }

            if( position.x  < 0.0 ){ // left


                if( objPosition.x < cropParentPosition.x ){
                    lostX = cropParentPosition.x - objPosition.x;
                    objPosition.x = cropParentPosition.x;
                }

            }else{ // right

////                if( hasSecondCropParent ){//} && cropParentPosition.x > cropParentPosition2.x ){
//                    cropParentPosition.x = cropParentPosition2.x;
////                }

                if( objPosition.x > cropParentPosition.x ){
                    lostX = objPosition.x - cropParentPosition.x;
                    objPosition.x = cropParentPosition.x;
                }

            }

            ui_texture_y += ui_texture_yscale * (lostY / effectiveScale.y) * 0.5;

            // this modifies rounding corners somehow? needs work - only works for certain size elemtns fixme
            OrigTexCoordOut.y += (lostY * 4.0);
        }
    }

    TexCoordOut = vec2( TexCoordIn.s / ui_texture_xscaler + ui_texture_x,   TexCoordIn.t / ui_texture_yscaler + ui_texture_y);
    gl_Position=objPosition;
}
