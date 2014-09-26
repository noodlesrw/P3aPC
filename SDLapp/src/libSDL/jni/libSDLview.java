package libSDL.jni;

import java.nio.Buffer;
import java.util.concurrent.Semaphore;
import java.nio.ByteBuffer;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Region;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import java.io.*;
public class libSDLview extends SurfaceView implements Runnable,SurfaceHolder.Callback
{
    static private ByteBuffer ScreenBuffer;
    private libSDL Owner;
    private Thread thread;
    static Boolean MainCalled=false;
    static Boolean Running=false;
    static Boolean WasRestarted=false;
    static SurfaceHolder holder = null;
    static libSDLview My = null; 
	long lasttime=0;  
	long FPS_lasttick=0;
	int FPS_count=0;
	static Bitmap ScreenBitmap = null;
	Paint ScreenPaint = new Paint();
	static Rect ViewportRect = new Rect();	// "viewport" means visible window on android screen
	static Rect ScreenRect = new Rect();	// "screen" means the emulated display
	static int ScreenWidth=640;		// emulated display W,H
	static int ScreenHeight=480;
	static Handler handler = null;
	static Semaphore MemLock=new Semaphore(1);

    
	private class SDLhandler extends Handler 
	{
	    public void handleMessage(Message Msg) 
	    {
    		ScreenBuffer=ByteBuffer.allocateDirect(Msg.arg1*Msg.arg2*2);    		
    		ScreenBitmap=Bitmap.createBitmap(Msg.arg1, Msg.arg2, Bitmap.Config.RGB_565);
    		MemLock.release();
    		super.handleMessage(Msg);
	    }
	}	
	
    public libSDLview(Context context) 
    { 
        super(context);
    	My=this;
        Owner=(libSDL)context;
        setFocusable(true); 
        setFocusableInTouchMode(true);
        handler = new SDLhandler(); 
        
        setOnKeyListener(new OnKeyListener() 
        {
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {

				if(keyCode==KeyEvent.KEYCODE_MENU)
		    		return false;
		    	
				if(event.getAction() == KeyEvent.ACTION_UP)
				{
					v.onKeyUp(keyCode, event);
				}
				if(event.getAction() == KeyEvent.ACTION_DOWN)
				{
					v.onKeyDown(keyCode, event);
				}
				if(event.getAction() == KeyEvent.ACTION_MULTIPLE)
				{
					v.onKeyMultiple(keyCode, event.getRepeatCount(), event);
				}
				return true;
			} 
		});
        requestFocus();
        holder = getHolder();
        holder.addCallback(this);
        holder.setKeepScreenOn(true);
        System.gc();
    }

    void ShowFPS(Canvas C)
    {
    	FPS_count++;
    	long curr=SystemClock.elapsedRealtime();
    	if(curr-FPS_lasttick>1000)
    	{
    		FPS_count=(int)(FPS_count*(curr-FPS_lasttick)/1000.0);
        	Log.v("libSDL", "FPS: "+FPS_count);
        	FPS_lasttick=curr;
        	FPS_count=0; 
    	}  
    }
    
    protected void myDraw(Canvas canvas) 
    {
    	if(ScreenBuffer!=null && ScreenBitmap!=null)
    	{
	    	if(libSDLproxy.libSDLupdatebuffer(ScreenBuffer,getWidth(),getHeight())==0)
	    		Owner.finish(); 
	       	else
	       	{
	       		synchronized (ScreenBitmap)
	       		{
       				ScreenBuffer.position(0);
       				ScreenBitmap.copyPixelsFromBuffer(ScreenBuffer);
	       			canvas.drawBitmap(ScreenBitmap, ScreenRect, ViewportRect, ScreenPaint);
	       		}
	      		if(libSDLconfig.CONF_DEBUG_FPS)
	      			ShowFPS(canvas);
	       	}
    	}
    	lasttime=SystemClock.elapsedRealtime();
    } 

    static void UpdateViewportRect()
    {
    	ViewportRect.left=0;
    	ViewportRect.top=0;
    	ViewportRect.right=My.getWidth()-1;
    	ViewportRect.bottom=My.getHeight()-1;
    	float ScreenAspect=ScreenWidth/(float)ScreenHeight;
    	float ViewportAspect=My.getWidth()/(float)My.getHeight();
    	if(ScreenAspect>ViewportAspect)	// add left-right borders
    	{
    		float Kx=My.getWidth()/(float)ScreenWidth;
    		ViewportRect.bottom=(int)(ScreenHeight*Kx)-1;
        	float dy=(My.getHeight()-(ViewportRect.bottom+1))/2;
        	ViewportRect.top+=dy;
        	ViewportRect.bottom+=dy;
    	} else
    	{
    		float Ky=My.getHeight()/(float)ScreenHeight;
    		ViewportRect.right=(int)(ScreenWidth*Ky)-1;
        	float dx=(My.getWidth()-(ViewportRect.right+1))/2;
        	ViewportRect.right+=dx;
        	ViewportRect.left+=dx;
    	}
		Canvas C=holder.lockCanvas();
		if(C!=null)
		{
			C.drawColor(255);  
			holder.unlockCanvasAndPost(C);
		}
    }
    
    /* Callback invoked when the surface dimensions change. */
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height) 
    {
    	UpdateViewportRect();
    	if(!Running)
        {
	    	thread = new Thread(this);
	    	Running = true;
	        thread.start();
        }
        if(WasRestarted)
        {
			new AlertDialog.Builder(Owner)
			.setTitle("SDL Application")
			.setMessage("Terminate program?")
			.setPositiveButton("Yes",	new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,	int which) {Owner.finish();}
			})
			.setNegativeButton("No",	new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,	int which) {}
			})
			.show();
        }
    } 
 
    /*
     * Callback invoked when the Surface has been created and is ready to be
     * used.
     */
    public void surfaceCreated(SurfaceHolder holder) 
    {
        // start the thread here so that we don't busy-wait in run()
        // waiting for the surface to be created
        if(!Running)
        {
	    	thread = new Thread(this);
	    	Running = true;
	        thread.start();
        }
		try {
			libSDLproxy.libSDLresume();	// in the case if we were paused 
		} catch (Throwable e) {
			/* do nothing */;
		}
    }

    /**
     * Standard window-focus override. Notice focus lost so we can pause on
     * focus lost. e.g. user switches to take a call.
     */
    @Override
    public void onWindowFocusChanged(boolean hasWindowFocus) {
        if (!hasWindowFocus) 
        {	
        	Running=false;
        	WasRestarted = true;
			libSDLproxy.libSDLpause(); 
        } else
        {
			libSDLproxy.libSDLresume(); 
        }
    }
    
    static public void DrawEverything()
    {
    	if(holder!=null)
    	{
			Canvas C=holder.lockCanvas();
			if(C!=null)
			{
				My.myDraw(C);  
				holder.unlockCanvasAndPost(C);
			}
    	}
    }

    public void run() 
    {
    	if(!MainCalled)
    	{
    		String CurrDir=Environment.getExternalStorageDirectory().getAbsolutePath();
    		if(CurrDir.length()<1 || (Environment.getExternalStorageState().compareTo(Environment.MEDIA_MOUNTED)!=0))
    			CurrDir=Environment.getDataDirectory().getAbsolutePath();
    		
    		CurrDir+="/SDL";
    		String workDir=CurrDir;
    		String defCmdLine=workDir+"/sdl -L . -m 16 -boot c -soundhw sb16 -hda 1Gb.img -usb -usbdevice tablet";
    		String defCmdFile=workDir+"/sdl.conf";
            String cmdLine;
    		Log.v("libSDL","Setting current directory to: "+workDir);
    		try {
    			InputStream ips=new FileInputStream(defCmdFile); 
                InputStreamReader ipsr=new InputStreamReader(ips);
                BufferedReader br=new BufferedReader(ipsr);
                workDir=br.readLine();
                cmdLine=br.readLine();
                br.close();
    		}
    		catch(Exception cEx){
    			Log.v("libSDL","sdl.conf not found.");
    			cmdLine=defCmdLine;
    		}
    		Log.v("libSDL","CmdLine:"+cmdLine);
    		MainCalled=true;
    		libSDLproxy.libSDLmain(workDir,cmdLine);
    		//CurrDir+"/sdl -L . -m 16 -boot c -soundhw sb16 -hda 1Gb.img -usb -usbdevice tablet"); 	
    	}

    	while(Running)
    	{
    		try {
				Thread.sleep(libSDLconfig.CONF_FORCED_REFRESH_PERIOD);
	       		Owner.updateOrientation();
	        	if(SystemClock.elapsedRealtime()-lasttime>libSDLconfig.CONF_FORCED_REFRESH_PERIOD)
	        			DrawEverything();	// force redraw only if really needed
			} catch (InterruptedException e) {}
    	}
    } 
    	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
//    	Log.v("SDLdn", event.toString());
    	if(libSDLconfig.CONF_TRACKBALL_AS_MOUSE)
    	{
    		if(keyCode == KeyEvent.KEYCODE_VOLUME_UP)
    		{
    			libSDL.TrackballAcceleration*=1.5;
    			if(libSDL.TrackballAcceleration>11)
        			libSDL.TrackballAcceleration=11;
    			return super.onKeyDown(keyCode, event);
    		}
    		if(keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
    		{
    			libSDL.TrackballAcceleration/=1.5;
    			if(libSDL.TrackballAcceleration<1.0/1.5)
    				libSDL.TrackballAcceleration=1.0/1.5;
    			return super.onKeyDown(keyCode, event);
    		}
    	}
    	libSDLproxy.libSDLnotifykey(1,keyCode);
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event) {
//    	Log.v("SDLmu", event.toString());
		return super.onKeyMultiple(keyCode, repeatCount, event);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
//    	Log.v("SDLup", event.toString());
    	libSDLproxy.libSDLnotifykey(0,keyCode);
		return super.onKeyUp(keyCode, event);
	}

    public void surfaceDestroyed(SurfaceHolder holder) {
        // we have to tell thread to shut down & wait for it to finish, or else
        // it might touch the Surface after we return and explode
    	Running = false;
    	WasRestarted = true;
        try {
           	libSDLproxy.libSDLpause(); 
			thread.join();
		} catch (Throwable t) {
		}
	    
	    Owner.CreateNotification();
    }
    
    public static Buffer AllocateBitmap(int width, int height)
    {
        ScreenWidth=width; ScreenHeight=height; 
   		ScreenRect.left=0;
    	ScreenRect.top=0;
    	ScreenRect.right=width-1;
    	ScreenRect.bottom=height-1;
    	UpdateViewportRect();
    	System.gc();
    	try
    	{
    		// Bitmap allocation can be made only from main thread (why???)
    		// so schedule a message and wait
    		ScreenBuffer=null;
    		ScreenBitmap=null;
    		Message Msg=new Message();
    		Msg.arg1=width;
    		Msg.arg2=height;

    		MemLock.acquire();
    		handler.sendMessage(Msg);	// this will release semaphore
    		MemLock.acquire();			// wait on it
    		MemLock.release();
    	} catch(Throwable t)
    	{
    		ScreenBuffer=null;
	    	ScreenBitmap=null;
    	}
		return ScreenBuffer;    	
    }
}   
