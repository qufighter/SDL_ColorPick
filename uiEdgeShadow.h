//
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uiEdgeShadow_h
#define ColorPick_iOS_SDL_uiEdgeShadow_h

/*

 typedef enum  {
 TOP,
 RIGHT,
 BOTTOM,
 LEFT
 } SQUARE_EDGE_ENUM;


 */

struct uiEdgeShadow{

    uiEdgeShadow(uiObject* parentObj, int initialEdge, float size){

       // Ux* uxInstance = Ux::Singleton();

//        uiObjectItself = new uiObject();
//        uiObjectItself->setBoundaryRect(&boundaries);

        if( size > 0 ){
            render_scale = size;
        }else{
            render_scale = 0.1;
        }

        // top edge....... or just an edge...  this is double edge

            shadow_container = new uiObject();

            top_shadow = new uiObject();

            //top_shadow->setBoundaryRect( 0.0, -0.05, 1.0, 0.1);
            top_shadow->hasBackground = true;
            Ux::setColor(&top_shadow->backgroundColor,0, 0, 0, 128);
            top_shadow->setRoundedCorners(0.5, 0.5, 0, 0);
            //uiObjectItself->addChild(top_shadow);
            shadow_container->addChild(top_shadow);


            top_shadow2 = new uiObject();
            //top_shadow2->setBoundaryRect( 0.0, 0.25, 1.0, 0.5);
            top_shadow2->hasBackground = true;
            Ux::setColor(&top_shadow2->backgroundColor,255, 255, 255, 128);
            top_shadow2->setRoundedCorners(0.5, 0.5, 0, 0);
            top_shadow->addChild(top_shadow2);

            top_shadow2->doesInFactRender = false; //addHighlight



//        shadow_container->hasBackground = true;
//        Ux::setColor(&shadow_container->backgroundColor,0, 255, 0, 128);

        parentObj->addBottomChild(shadow_container);

        top_shadow->setCropParentRecursive(shadow_container);

        shadow_container->myUiController = this;


        current_edge = initialEdge;

        resize();

    }

    uiObject* shadow_container;
    uiObject* top_shadow; // this its the uiEdgeShadow, I would use self->
    uiObject* top_shadow2;

    //uiObject* uiObjectItself; // no real inheritance here, this its the uiEdgeShadow, I would use self->

    int current_edge;
    float render_scale = 0.1;

    void shadowOnly(){
        top_shadow2->doesInFactRender = false;
    }

    void addHighlight(){
        top_shadow2->doesInFactRender = true;
    }

    void setOpacity(Uint8 newOpacity){
        top_shadow->backgroundColor.a = newOpacity;
    }


    void resize(){
        resize(current_edge);
    }

    void resize(int edgeToUse){


        //uiObjectItself->setBoundaryRect(&boundaries);
        //hexValueText->setChildNodeDirection(TEXT_DIR_ENUM::TTB, false); // see TEXT_DIR_ENUM
        //Ux* uxInstance = Ux::Singleton();

        float halfRenderScale = render_scale * 0.5;


        switch(edgeToUse){
            case SQUARE_EDGE_ENUM::TOP:{

                shadow_container->setBoundaryRect(  0.0, -halfRenderScale, 1.0, halfRenderScale );

                //top_shadow->setBoundaryRect(  0.0, -halfRenderScale, 1.0, render_scale );
                top_shadow->setBoundaryRect(  0.0, 0.0, 1.0, 2.0 );
                top_shadow2->setBoundaryRect( 0.0,  0.25, 1.0, 0.5 );
                top_shadow->setRoundedCorners( 0.5, 0.5, 0, 0);
                top_shadow2->setRoundedCorners(0.5, 0.5, 0, 0);
                break;
            }
            case SQUARE_EDGE_ENUM::BOTTOM:{

                shadow_container->setBoundaryRect(  0.0, 1.0, 1.0, halfRenderScale );

                //top_shadow->setBoundaryRect(  0.0, 1.0-halfRenderScale, 1.0, render_scale );
                top_shadow->setBoundaryRect(  0.0, -1.0, 1.0, 2.0 );
                top_shadow2->setBoundaryRect( 0.0,  0.25, 1.0, 0.5 );
                top_shadow->setRoundedCorners( 0, 0, 0.5, 0.5);
                top_shadow2->setRoundedCorners(0, 0, 0.5, 0.5);
                break;
            }
            case SQUARE_EDGE_ENUM::RIGHT:{


                shadow_container->setBoundaryRect(  1.0, 0.0, halfRenderScale, 1.0 );

                //top_shadow->setBoundaryRect(  1.0-halfRenderScale, 0.0, render_scale, 1.0);
                top_shadow->setBoundaryRect(  -1.0, 0.0, 2.0, 1.0 );
                top_shadow2->setBoundaryRect(  0.25, 0.0, 0.5, 1.0);
                top_shadow->setRoundedCorners(  0, 0.5, 0.5, 0);
                top_shadow2->setRoundedCorners( 0, 0.5, 0.5, 0);
                break;
            }
            case SQUARE_EDGE_ENUM::LEFT:{

                shadow_container->setBoundaryRect( -halfRenderScale, 0.0, halfRenderScale, 1.0 );

                //top_shadow->setBoundaryRect(  -halfRenderScale, 0.0, render_scale, 1.0);
                top_shadow->setBoundaryRect(  0.0, 0.0, 2.0, 1.0 );
                top_shadow2->setBoundaryRect(  0.25, 0.0, 0.5, 1.0);
                top_shadow->setRoundedCorners( 0.5, 0, 0, 0.5);
                top_shadow2->setRoundedCorners(0.5, 0, 0, 0.5);
                break;
            }
        }
    }

    void update(){

    }

};


#endif
