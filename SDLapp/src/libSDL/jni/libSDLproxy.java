package libSDL.jni;

import java.nio.Buffer;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.SystemClock;

public class libSDLproxy 
{ 
    private static 	long lasttime=0;	// used to skip too frequent updates
    
    static {
    		System.loadLibrary("nativeapp");
    }
    
    libSDLproxy()
    {
    	// unused
    }
    
    /*
     * Initialize SDL, create parallel thread and call there application's main() function
     * pass current dir as param
     */
    public native static int libSDLmain(String CurDir, String Args); 
    
    /*
     * Return 1 (version number), used to check .SO file compatibility
     */
    public native static int libSDLcheckloaded();
    
    /*
     * Cleanup SDL and terminate application's main() function
     */
    public native static void libSDLcleanup();
        
    /*
     * Pause SDL thread if not paused yet and resume it if it was paused earlier. Otherwise do nothing.
     */
    public native static void libSDLpause();
    public native static void libSDLresume();

    /*
     * Return ==1 if current mode is portrait (W<=H), -1 if SDL not initialized, 0 otherwise
     */
    public native static int libSDLisportrait();

    /*
     * Simulate mouse. Flag: 0=move, 1=press, 2=release, button=1,2,3 (or -1 if ignore), x,y (-1 if ignore), absolute
     */
    public native static void libSDLnotifytouch(int flag, int button, int x, int y);

    /*
     * x,y - joystick positions -32767..+32767. Button - button state: -1 ignored, 1 down, 0 up
     */
    public native static void libSDLnotifyjoystick(int x, int y, int button);
    
    /*
     * Simulate mouse via track pad/ball. Flag: 0=move, 1=press, 2=release, button=1,2,3 (or -1 if ignore), x,y (-1 if ignore) = deltas
     * press&release - identical to libSDLnotifytouch
     */
    public native static void libSDLnotifytrackpad(int flag, int button, int x, int y);
    
    /*
     * Notify on keypress flag: 0==released, 1=pressed
     */
    public native static void libSDLnotifykey(int flag, int code);
    
    /*    
     * Update the passed buffer with the screen data
     */
    public native static int libSDLupdatebuffer(Buffer buffer,int w, int h);
    
    /* 
     * Called from C++ side to force redraw
     */
    public static void javaSDLnotifyneedupdate() 
    {
    	long curr=SystemClock.elapsedRealtime();
    	if(libSDLconfig.CONF_NO_FPS_LIMIT || curr-lasttime>30)	// limit to 30 FPS or not	
    	{
    		libSDLview.DrawEverything();
    		lasttime=curr;
    	} 
    }

    /*
     * Called from C part to notify that display mode has changed. Return "video mem" buffer
     * allocated with allocateDirect(w*h*2)	(16 bpp modes only)
     */
    public static Buffer javaSDLnotifymodechanged(int w, int h)
    {
    	return libSDLview.AllocateBitmap(w,h);
    }
    
    /*
     * Initialize audio. Audio data is 16 bit, channels, freq
     */
    static AudioTrack Aud=null;
    public static int javaSDLinitaudio(int freq, int channels)
    {
    	int BuffSize=AudioTrack.getMinBufferSize(freq, channels==1?AudioFormat.CHANNEL_OUT_MONO:AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT);
    	Aud=new AudioTrack(AudioManager.STREAM_MUSIC,freq,channels==1?AudioFormat.CHANNEL_OUT_MONO:AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT,BuffSize,AudioTrack.MODE_STREAM);
		return BuffSize;    
    }    
    /*
     * Play the given audio data
     */
    public static void javaSDLwriteaudio(short[] audioData, int sizeInBytes)
    {
    	if(Aud!=null)
    	{
    		Aud.play();
    		Aud.write(audioData, 0, sizeInBytes/2);
    	}
    }
} 
