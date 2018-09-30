package com.vidsbee.colorpicksdl;

import org.libsdl.app.SDLActivity;


//import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
//import android.graphics.BitmapFactory;
import android.net.Uri;
//import android.os.Bundle;
import android.provider.MediaStore;
//import android.view.View;
//import android.widget.Button;
//import android.widget.ImageView;


import android.util.Log;


public class SDLColorPickActivity extends SDLActivity {

    private static int RESULT_LOAD_IMAGE = 1;

    public static native void loadThisImagePath(String path);

    private static String lastResult = "";

    public void beginImageSelector(){

        lastResult = "WAITING";

        Intent i = new Intent(
                              Intent.ACTION_PICK,
                              android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);

        startActivityForResult(i, RESULT_LOAD_IMAGE);
        

    }

    public static String checkForImageSelectorCompleted(){

        if( lastResult.equals("WAITING")){
            return null;
        }
        return lastResult;

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == RESULT_LOAD_IMAGE && resultCode == RESULT_OK && null != data) {
            Uri selectedImage = data.getData();
            String[] filePathColumn = { MediaStore.Images.Media.DATA };

            Cursor cursor = getContentResolver().query(selectedImage,
                                                       filePathColumn, null, null, null);
            cursor.moveToFirst();

            int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
            String picturePath = cursor.getString(columnIndex);

            cursor.close();

            Log.v("SDL ? ", "filepath: " + picturePath);

            lastResult = picturePath;

            //SDLColorPickActivity.loadThisImagePath(picturePath); // calls native function...

            //return picturePath;

//            ImageView imageView = (ImageView) findViewById(R.id.imgView);
//            imageView.setImageBitmap(BitmapFactory.decodeFile(picturePath));




        }else{
            lastResult = "";
        }


    }

}

