//
//  uxHueGradientData.h
//  ColorPick SDL
//
//  Created by Sam Larison on 1/7/18.
//
//

#ifndef ColorPick_iOS_SDL_uxHueGradientData_h
#define ColorPick_iOS_SDL_uxHueGradientData_h


struct uxHueGradientData{

    uxHueGradientData(){
        generateHueGradient();
    }

    const static int totalColors = 1530; // so this may not be defined with a value???
    SDL_Color manyColors[totalColors];// *totalColorsPer * 6 // we really want this to be static and allocated once....

    void generateHueGradient(){

        int totalColorsPer = 255;
        int stopPoint = totalColorsPer;
        int counter = 0;
        int current_r = 255;
        int current_g = 0;
        int current_b = 0;

        // 6 chunks of 0-255 rainbow

        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        while( counter < totalColorsPer ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_b+=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 2;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_r-=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 3;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_g+=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 4;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_b-=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 5;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_r+=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);

        stopPoint = totalColorsPer * 6;
        while( counter < stopPoint ){
            Ux::setColor(&manyColors[counter], current_r, current_g, current_b, 255);
            current_g-=1;
            counter++;
        }
        SDL_Log("SPLIT %i %i %i", current_r, current_g, current_b);
    }

};


#endif
