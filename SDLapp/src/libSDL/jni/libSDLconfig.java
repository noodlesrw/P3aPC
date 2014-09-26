package libSDL.jni;

import android.content.pm.ActivityInfo;

public class libSDLconfig 
{
	/*
	 * Default screen orientation
	 */
	static int CONF_DEFAULT_ORIENTATION=ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
	
	/*
	 * Simulate trackpad with touchscreen. I.e. no mouse moves with pressed LB
	 */
	static Boolean CONF_TOUCH_AS_TRACKPAD=true;
	
	/*
	 * When true - use trackball to simulate mouse move, left button down. Otherwise simulate arrow keys + enter
	 */
	static Boolean CONF_TRACKBALL_AS_MOUSE=true;
	
	/*
	 * Trackball speed (trackball data is multiplied by this value), use volume up/down to change speed
	 * useful only if CONF_TRACKBALL_AS_MOUSE=true
	 */
	static double CONF_TRACKBALL_MULTIPLY=16.0;
	
	/*
	 * true == don't listen to accelerometer (and ignore all other accelerometer settings) 
	 */
	static Boolean CONF_DISABLE_ACCELEROMETER=true;

	/*
	 * true == accelerometer simulates SDL joystick, otherwise - arrow keys. 
	 * Note: joy buttons currently are not implemented
	 */
	static Boolean CONF_ACCELEROMETER_AS_JOYSTICK=true;
	
	/*
	 * Used to move "joystick" center position vertically
	 */
	static int CONF_JOYSTICK_Y_DELTA=15000;
	
	/*
	 * Configure forced refresh every xxx msec. Would cause blinking if app does not call SDL_UpdateRects.
	 * Also used to rotate display after changing mode. 
	 */
	static int CONF_FORCED_REFRESH_PERIOD=500;
	
	/*
	 * true - do not limit max FPS to 30, otherwise javaSDLnotifyneedupdate would not 
	 * update screen faster then 30 times per sec. 'false' speeds up things a bit.
	 */
	static Boolean CONF_NO_FPS_LIMIT=false;

	/*
	 * Output FPS counter to log
	 */
	static Boolean CONF_DEBUG_FPS=true;

}
