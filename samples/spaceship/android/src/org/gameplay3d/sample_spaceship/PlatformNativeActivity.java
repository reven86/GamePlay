
package org.gameplay3d.sample_spaceship;

import android.util.Log;
import android.view.Gravity;
import android.widget.Toast;

public class PlatformNativeActivity extends GooglePlaySocial {
    // displacement of the toast
    private static final int TOAST_NUDGE_Y = 0;
	
	void showToast(String string) {	
        Toast t = Toast.makeText(this, string, Toast.LENGTH_LONG);
        t.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, TOAST_NUDGE_Y);
        t.getView().setBackgroundResource(0);
        t.show();
	}
}