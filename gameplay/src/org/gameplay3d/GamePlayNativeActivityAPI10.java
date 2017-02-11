package org.gameplay3d;

import android.app.NativeActivity;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.util.SparseArray;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Display;
import android.view.WindowManager;
import android.view.OrientationEventListener;
import android.provider.MediaStore;
import android.database.Cursor;
import android.os.Environment;
import android.content.ContentUris;
import android.content.Context;
import android.net.Uri;
import android.content.Intent;

/**
 * GamePlay native activity extension for Android platform.
 * 
 * Handles any platform features that cannot be handled natively in PlatformAndroid.cpp 
 * 
 * Developers may choose to directly modify/extend this for any addition platform features 
 * that are not offered directly the gameplay3d framework such as platform events, access to 
 * android user-interface, life-cycle events for saving game state and custom plug-ins/extensions.
 */
public class GamePlayNativeActivity extends NativeActivity {
        
    private static final String TAG = "GamePlayNativeActivity";
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);

        orientationListener = new OrientationEventListener(this) {
            public void onOrientationChanged(int orientation) {
                if (orientation != OrientationEventListener.ORIENTATION_UNKNOWN) {
                    WindowManager mWindowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);

                    Display display = mWindowManager.getDefaultDisplay();
                    int rotation = display.getRotation();
                    screenOrientationChanged(rotation);
                }
            }
        };
    }

    @Override
    protected void onResume() {
        super.onResume();
        orientationListener.enable();
    }

    @Override
    protected void onPause() {
        orientationListener.disable();
        super.onPause();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);

        //Log.i(TAG, "onNewIntent: " + getCallingPackage() + " " + getArguments());

        openURLEvent(getArguments());
    }        

    // JNI calls to PlatformAndroid.cpp
    private static native void gamepadEventConnectedImpl(int deviceId, int buttonCount, int joystickCount, int triggerCount, String deviceName);
    private static native void gamepadEventDisconnectedImpl(int deviceId);
    private static native void screenOrientationChanged(int orientation);
    private static native void openURLEvent(String url);


    // IAB
    public native void setIABEnabled();
    public native int isItemConsumable(String sku);
    public native void itemRestored(String sku, long purchaseTime, String orderId);
    public native void itemPurchased(String sku, long purchaseTime, String orderId);
    public native void itemPurchaseFailed(String sku, int error, String message);
    public native void getProductsFailed(int error, String message);
    public native void productValidated(String sku, String price, String title, String description, String priceAmount, String priceCurrency);
    public native void finishProductsValidation();

    public void queueSkuDetailsRequest(String sku) {};
    public void flushSkuDetailsQueue() {};
    public void purchaseItem(final String sku) {};
    public void restorePurchases() {};

    private OrientationEventListener orientationListener;


    public String getArguments()
    {
        Uri arg = getIntent().getData();
        if (arg != null)
            return "file://"+getPath(this, arg);
        return "";
    }

    /**
     * Get a file path from a Uri. This will get the the path for Storage Access
     * Framework Documents, as well as the _data field for the MediaStore and
     * other file-based ContentProviders.
     *
     * @param context The context.
     * @param uri The Uri to query.
     * @author paulburke
     */
    public static String getPath(final Context context, final Uri uri) {

      // MediaStore (and general)
      if ("content".equalsIgnoreCase(uri.getScheme())) {
        return getDataColumn(context, uri, null, null);
      }
      // File
      else if ("file".equalsIgnoreCase(uri.getScheme())) {
        return uri.getPath();
      }

      return null;
    }

    /**
     * Get the value of the data column for this Uri. This is useful for
     * MediaStore Uris, and other file-based ContentProviders.
     *
     * @param context The context.
     * @param uri The Uri to query.
     * @param selection (Optional) Filter used in the query.
     * @param selectionArgs (Optional) Selection arguments used in the query.
     * @return The value of the _data column, which is typically a file path.
     */
    public static String getDataColumn(Context context, Uri uri, String selection,
        String[] selectionArgs) {

      Cursor cursor = null;
      final String column = "_data";
      final String[] projection = {
          column
      };

      try {
        cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs,
            null);
        if (cursor != null && cursor.moveToFirst()) {
          final int column_index = cursor.getColumnIndexOrThrow(column);
          return cursor.getString(column_index);
        }
      } finally {
        if (cursor != null)
          cursor.close();
      }
      return null;
    }


    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is ExternalStorageProvider.
     */
    public static boolean isExternalStorageDocument(Uri uri) {
      return "com.android.externalstorage.documents".equals(uri.getAuthority());
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is DownloadsProvider.
     */
    public static boolean isDownloadsDocument(Uri uri) {
      return "com.android.providers.downloads.documents".equals(uri.getAuthority());
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is MediaProvider.
     */
    public static boolean isMediaDocument(Uri uri) {
      return "com.android.providers.media.documents".equals(uri.getAuthority());
    }
}
