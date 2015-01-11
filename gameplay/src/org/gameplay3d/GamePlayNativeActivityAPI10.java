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
    
    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("gameplay");
    }
    
    private static final String TAG = "GamePlayNativeActivity";
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
    }
        
    // JNI calls to PlatformAndroid.cpp
    private static native void gamepadEventConnectedImpl(int deviceId, int buttonCount, int joystickCount, int triggerCount, String deviceName);
    private static native void gamepadEventDisconnectedImpl(int deviceId);

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
}
