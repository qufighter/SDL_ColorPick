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

import android.Manifest;

// see     compile 'com.android.support:support-v4:+' in build.gradle....
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;
import android.content.pm.PackageManager;

import android.util.Log;


public class SDLColorPickActivity extends SDLActivity {

    private static int RESULT_LOAD_IMAGE = 1;

    public static native void loadThisImagePath(String path);

    private static String lastResult = "";

    final private static int MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE = 1029;

    public void openWebsiteUrl(String url){
        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                    && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay! Do the
                    //  task you need to do.
                    beginImageSelector();
                } else {
                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                }
                return;
            }

                // other 'case' lines to check for other
                // permissions this app might request.
        }
    }


    public void beginImageSelector(){

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
            != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                                              new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                                              MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE);
            return;
        }

        lastResult = "WAITING";

        Intent i = new Intent(
                              Intent.ACTION_PICK,
                              android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);

        startActivityForResult(i, RESULT_LOAD_IMAGE);
        

    }

    public static String checkForImageSelectorCompleted(){
        if( lastResult == null ){
            return ""; // good luck opening this path ;)  but the error handling should kick in...
        }
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

