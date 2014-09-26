package libSDL.jni;

import android.app.Application;

public class libSDLapp extends Application 
{
	@Override
	public void onTerminate() {
		super.onTerminate();
        try {
        	libSDLproxy.libSDLcleanup();	// just to be 100% sure to cleanup, but this func is rarely called
		} catch (Throwable e) {
			/* do nothing as we are exiting anyway */;
		}
	}
}
