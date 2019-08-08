package org.gameplay3d;

import android.app.NativeActivity;
import android.content.Context;
import android.content.res.Configuration;
import android.hardware.input.InputManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.util.SparseArray;
import android.view.Display;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.OrientationEventListener;
import android.provider.MediaStore;
import android.database.Cursor;
import android.os.Environment;
import android.content.ContentUris;
import android.content.Context;
import android.net.Uri;
import android.provider.DocumentsContract;

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
    
    private class GamePlayInputDeviceListener
        implements InputManager.InputDeviceListener {
        @Override
        public void onInputDeviceAdded(int deviceId) {
            getGamepadDevice(deviceId);
        }

        @Override
        public void onInputDeviceRemoved(int deviceId) {
            InputDevice device = _gamepadDevices.get(deviceId);
            if (device != null) {
                _gamepadDevices.remove(deviceId);
                Log.v(TAG, "Gamepad disconnected:id=" + deviceId);
                gamepadEventDisconnectedImpl(deviceId);
            }
        }

        @Override
        public void onInputDeviceChanged(int deviceId) {
        }
    }

    private static final String TAG = "GamePlayNativeActivity";
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        _gamepadDevices = new SparseArray<InputDevice>();
        Log.v(TAG, "Build version: " + Build.VERSION.SDK_INT);
        if (Build.VERSION.SDK_INT >= 16) {
            _inputManager = (InputManager)getSystemService(Context.INPUT_SERVICE);
            _inputDeviceListener = new GamePlayInputDeviceListener();
        }

        if (Build.VERSION.SDK_INT >= 19)
        {
            View decorView = getWindow().getDecorView();
            int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                            View.SYSTEM_UI_FLAG_FULLSCREEN |
                            0x00000800; // View.SYSTEM_UI_FLAG_IMMERSIVE;
            decorView.setSystemUiVisibility(uiOptions);
        }

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
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }
    
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    } 
    
    @Override
    protected void onResume() {
        super.onResume();
        orientationListener.enable();
        if (_inputManager != null) {
            _inputManager.registerInputDeviceListener(_inputDeviceListener, null);
            int[] ids = InputDevice.getDeviceIds();
            for (int i = 0; i < ids.length; i++) {
                getGamepadDevice(ids[i]);
            }
        }
    }
    
    @Override
    protected void onPause() {
        orientationListener.disable();
        if (_inputManager != null) {
            _inputManager.unregisterInputDeviceListener(_inputDeviceListener);
        }
        super.onPause();
    }
    
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);

        String args = getArguments();
        Log.i(TAG, "onNewIntent: " + getCallingPackage() + " " + args);

        openURLEvent(args);
    }        


    private void onGamepadConnected(int deviceId, String deviceName) {
        int buttonCount = 17;
        int joystickCount = 2;
        int triggerCount = 2;
        
        Log.v(TAG, "Gamepad connected:id=" + deviceId + ", name=" + deviceName);
        
        gamepadEventConnectedImpl(deviceId, buttonCount, joystickCount, triggerCount, deviceName);
    }
    
    private InputDevice getGamepadDevice(int deviceId) {
        InputDevice device = _gamepadDevices.get(deviceId);
        if (device == null) {
            device = InputDevice.getDevice(deviceId);
            if (device == null)
                return null;
            int sources = device.getSources();
            if (((sources & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD) || 
                ((sources & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK)) {
                _gamepadDevices.put(deviceId, device);
                
                onGamepadConnected(deviceId, device.getName());
            }
        }
        return device;
    }
    
    
    // JNI calls to PlatformAndroid.cpp
    private static native void gamepadEventConnectedImpl(int deviceId, int buttonCount, int joystickCount, int triggerCount, String deviceName);
    private static native void gamepadEventDisconnectedImpl(int deviceId);
    private static native void screenOrientationChanged(int orientation);
    private static native void openURLEvent(String url);

    // IAB
    public native void setIABEnabled();
    public native int isItemConsumable(String sku);
    public native int isSubscription(String sku);
    public native void itemPurchased(String sku, long purchaseTime, String orderId);
    public native void itemPurchaseFailed(String sku, int error, String message);
    public native void getProductsFailed(int error, String message);
    public native void productValidated(String sku, String price, String title, String description, String priceAmount, String priceCurrency);
    public native void finishProductsValidation();

    public void queueSkuDetailsRequest(String sku) {};
    public void flushSkuDetailsQueue() {};
    public void purchaseItem(final String sku) {};
    public void restorePurchases() {};

    private InputManager _inputManager = null;
    private SparseArray<InputDevice> _gamepadDevices;
    private OrientationEventListener orientationListener;
    GamePlayInputDeviceListener _inputDeviceListener = null;


    public String getArguments()
    {
        Uri arg = getIntent().getData();
        Log.i(TAG, "getArguments: " + arg);
        if (arg != null)
            return getPath(this, arg);
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

      final boolean isKitKat = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;

        // DocumentProvider
        if (isKitKat && DocumentsContract.isDocumentUri(context, uri)) {
            // ExternalStorageProvider
            if (isExternalStorageDocument(uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                if ("primary".equalsIgnoreCase(type)) {
                    return "file://"+Environment.getExternalStorageDirectory() + "/" + split[1];
                }

                // TODO handle non-primary volumes
            }
            // DownloadsProvider
            else if (isDownloadsDocument(uri)) {

                final String id = DocumentsContract.getDocumentId(uri);
                final Uri contentUri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));

                return "file://"+getDataColumn(context, contentUri, null, null);
            }
            // MediaProvider
            else if (isMediaDocument(uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                Uri contentUri = null;
                if ("image".equals(type)) {
                    contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                } else if ("video".equals(type)) {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                } else if ("audio".equals(type)) {
                    contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                }

                final String selection = "_id=?";
                final String[] selectionArgs = new String[] {
                        split[1]
                };

                return "file://"+getDataColumn(context, contentUri, selection, selectionArgs);
            }
        }
      // MediaStore (and general)
      if ("content".equalsIgnoreCase(uri.getScheme())) {
        return "file://"+getDataColumn(context, uri, null, null);
      }
      // File
      if ("file".equalsIgnoreCase(uri.getScheme())) {
        return "file://"+uri.getPath();
      }

      return uri.toString();
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
          final int column_index = cursor.getColumnIndex(column);
          return column_index >= 0 ? cursor.getString(column_index) : null;
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
