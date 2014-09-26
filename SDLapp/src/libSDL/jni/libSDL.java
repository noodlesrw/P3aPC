package libSDL.jni;

import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

public class libSDL extends Activity 
{
	static int PrevOrientation=-1;
	int MouseMode=1;	// 1 == left mouse, 2 == middle (not implemented), 3 == right
	libSDLview SDLview;
	SensorManager sManager;
	Boolean oldSimLeft=false, oldSimRight=false, oldSimUp=false, oldSimDown=false;
	static public double TrackballAcceleration=1.0;	// Trackball acceleration coeff. 
    Boolean TrackballKillMoves=false;   // used to filter extra mouse moves on desire when trackball is pressed
    static NotificationManager mNotificationManager;
    static int NOTIFY_ID = 12300;
    int TP_DeltaX=0;	// Used in CONF_TOUCH_AS_TRACKPAD mode
    int TP_DeltaY=0;
	
	@Override
	protected void onDestroy() {
        try {
        	if(isFinishing())
        	{
        		mNotificationManager.cancel(NOTIFY_ID);
            	libSDLproxy.libSDLcleanup();
        	}
        	else
        		libSDLproxy.libSDLpause();
		} catch (Throwable e) {
			/* do nothing, as we can't know we are exiting app or just screen rotated */;
		}
		super.onDestroy();
	}

	@Override
	protected void onPause() {
        try {
        	libSDLproxy.libSDLpause();
		} catch (Throwable e) {
			/* do nothing */;
		}
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		try {
			libSDLproxy.libSDLresume();
		} catch (Throwable e) {
			/* do nothing */;
		}
	}

	private SensorEventListener sensorEventListener = new SensorEventListener() 
	{
		public void onAccuracyChanged(Sensor sensor, int accuracy) 
		{
        } 
        public void onSensorChanged(SensorEvent e) 
        {
          synchronized (this) 
          {
//              System.out.println("Sensor event: "+e.values[0]+" "+e.values[1]+" "+e.values[2]+" ");
              float AbsX=e.values[0];
              float AbsY=e.values[1];
              float AbsZ=e.values[2];
              if(AbsX<0)
            	  AbsX=-AbsX;
              if(AbsY<0)
            	  AbsY=-AbsY;
              if(AbsZ<0)
            	  AbsZ=-AbsZ; 
              
              Boolean NeedX=false; 
              Boolean NeedY=false;
              if(AbsX>2.5)
            	  NeedX=true;
              if(AbsY>2.5)
            	  NeedY=true;
              
          	  Boolean SimLeft=false, SimRight=false, SimUp=false, SimDown=false;  
              
              if(libSDLproxy.libSDLisportrait()!=0)
              {
	              if(NeedX && e.values[0]>0)
	              {  
//	            	  System.out.println("Sim: left");
	            	  SimLeft=true;
	              }
	              if(NeedX && e.values[0]<0)
	              {
//	            	  System.out.println("Sim: right");
	            	  SimRight=true;
	              }
	              if(NeedY && e.values[1]<0)
	              {
//	            	  System.out.println("Sim: down");
	            	  SimDown=true;
	              }
	              if(NeedY && e.values[1]>0)
	              {
//	            	  System.out.println("Sim: up");
	            	  SimUp=true;
	              }
              } else
              {
	              if(NeedX && e.values[0]>0)
	              {
//	            	  System.out.println("Sim: down");
	            	  SimDown=true;
	              }
	              if(NeedX && e.values[0]<0)
	              {
//	            	  System.out.println("Sim: up");
	            	  SimUp=true;
	              }
	              if(NeedY && e.values[1]<0)
	              {
//	            	  System.out.println("Sim: left");
	            	  SimLeft=true;
	              }
	              if(NeedY && e.values[1]>0)
	              {
//	            	  System.out.println("Sim: right");
	            	  SimRight=true;
	              }
              } 

              if(!libSDLconfig.CONF_ACCELEROMETER_AS_JOYSTICK)
              {
	              if(oldSimLeft!=SimLeft)
	                	libSDLproxy.libSDLnotifykey(SimLeft?1:0,KeyEvent.KEYCODE_DPAD_LEFT);
	              if(oldSimRight!=SimRight)
	                	libSDLproxy.libSDLnotifykey(SimRight?1:0,KeyEvent.KEYCODE_DPAD_RIGHT);
	              if(oldSimUp!=SimUp)
	                	libSDLproxy.libSDLnotifykey(SimUp?1:0,KeyEvent.KEYCODE_DPAD_UP);
	              if(oldSimDown!=SimDown)
	                	libSDLproxy.libSDLnotifykey(SimDown?1:0,KeyEvent.KEYCODE_DPAD_DOWN);
              } else
              {
                  if(libSDLproxy.libSDLisportrait()!=0)
                	  libSDLproxy.libSDLnotifyjoystick(-(int)(e.values[0]*3276)-libSDLconfig.CONF_JOYSTICK_Y_DELTA,-(int)(e.values[1]*3276),-1);
                  else
                	  libSDLproxy.libSDLnotifyjoystick((int)(e.values[1]*3276)-libSDLconfig.CONF_JOYSTICK_Y_DELTA,(int)(e.values[0]*3276),-1);
              }
              
          	  oldSimLeft=SimLeft; oldSimRight=SimRight; oldSimUp=SimUp; oldSimDown=SimDown;  
          }      
        }
    }; 
 
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
    	int vers=0;
    	super.onCreate(savedInstanceState);
    	Boolean Ok=true;    	
    	
        try {
			vers=libSDLproxy.libSDLcheckloaded();
		} catch (Throwable e) {
			Ok=false;
			new AlertDialog.Builder(this)
			.setTitle("Error")
			.setMessage("Cannot load SDL library")
			.setNeutralButton("Ok",	new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,	int which) {finish();}
			}).show();
		}
		if(Ok)
		{
			if(vers!=2)
			{
				new AlertDialog.Builder(this)
				.setTitle("Error")
				.setMessage("Incompatible SDL library version")
				.setNeutralButton("Ok",	new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog,	int which) {finish();}
				}).show();
			} else
			{			
		        requestWindowFeature(Window.FEATURE_NO_TITLE);  
		        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,   
		                             WindowManager.LayoutParams.FLAG_FULLSCREEN |
		                             WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		        SDLview = new libSDLview(this);
		        setContentView(SDLview);
		        
		        sManager = (SensorManager)getSystemService( SENSOR_SERVICE  );
		        if(!libSDLconfig.CONF_DISABLE_ACCELEROMETER)
		        {
			        List<Sensor> liste = sManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
			        for(int a=0;a<liste.size();a++) {
			          System.out.println("Sensor "+a+": "+liste.get(a).getName());
	 		          sManager.registerListener(sensorEventListener, liste.get(0), SensorManager.SENSOR_DELAY_NORMAL); 
			        }
		        }
		        setDefaultKeyMode(DEFAULT_KEYS_DISABLE);
				setRequestedOrientation(libSDLconfig.CONF_DEFAULT_ORIENTATION);
			} 
		}
    }

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		updateOrientation();
	}   

	public void updateOrientation()
	{
		int CurrOrientation=libSDLproxy.libSDLisportrait();
		
		if(PrevOrientation!=CurrOrientation)
		{
			if(CurrOrientation==0)
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
			else
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		} 
	} 

	@Override
	public boolean onTouchEvent(MotionEvent event) 
	{
		float scrX=event.getX()-libSDLview.ViewportRect.left; 
		float scrY=event.getY()-libSDLview.ViewportRect.top;
		scrX*=libSDLview.ScreenWidth/((float)libSDLview.ViewportRect.right-libSDLview.ViewportRect.left+1);
		scrY*=libSDLview.ScreenHeight/((float)libSDLview.ViewportRect.bottom-libSDLview.ViewportRect.top+1);
        int numpointers = 1;
        try 
        {
        	numpointers = event.getPointerCount();
        } catch(Throwable e)
        {
        }
        int action = event.getAction();
        if(numpointers==1)
        {
	        switch(action)
	        {
	        case MotionEvent.ACTION_DOWN:
	        	if(libSDLconfig.CONF_TOUCH_AS_TRACKPAD)	// immediately simulate mouse button up
	        	{
	        		TP_DeltaX=(int)scrX;
	        		TP_DeltaY=(int)scrY;
		        	libSDLproxy.libSDLnotifytrackpad(1,MouseMode,-1,-1);	        		
		        	try {
						Thread.sleep(50);	// add small delay, otherwise it may be ignored as jitter
					} catch (InterruptedException e) {
					}
		        	libSDLproxy.libSDLnotifytrackpad(2,MouseMode,-1,-1);
	        	} else
	        	{
	        		TP_DeltaX=0;
	        		TP_DeltaY=0;
		        	libSDLproxy.libSDLnotifytouch(0,-1,(int)scrX,(int)scrY);
		        	libSDLproxy.libSDLnotifytouch(1,MouseMode,-1,-1);	        		
	        	}
	        	break;
	        case MotionEvent.ACTION_UP:
	        	if(!libSDLconfig.CONF_TOUCH_AS_TRACKPAD)
	        	{
	        		libSDLproxy.libSDLnotifytouch(0,-1,(int)scrX,(int)scrY);
	        		libSDLproxy.libSDLnotifytouch(2,MouseMode,-1,-1);
	        	}
	        	MouseMode=1;	// restore LB mode
	        	break;
	        default:
	        	if(libSDLconfig.CONF_TOUCH_AS_TRACKPAD)
	        	{
		        	libSDLproxy.libSDLnotifytrackpad(0,-1,(int)scrX-TP_DeltaX,(int)scrY-TP_DeltaY);
	        		TP_DeltaX=(int)scrX;
	        		TP_DeltaY=(int)scrY;
	        	}
	        	else
	        		libSDLproxy.libSDLnotifytouch(0,-1,(int)scrX,(int)scrY);
	        	break;
	        };
        } else
        {
	        switch(action)
	        {
	        case MotionEvent.ACTION_DOWN:
	        	// on 2 fingers down: simulate mouse move with first finger
	        	libSDLproxy.libSDLnotifytouch(2,MouseMode,-1,-1);	// release 1st mouse button
	        	if(!libSDLconfig.CONF_TOUCH_AS_TRACKPAD)	
	        		libSDLproxy.libSDLnotifytouch(0,-1,(int)scrX,(int)scrY);
	        	break;
	        case MotionEvent.ACTION_UP:
	        	// 2 fingers, second up: switch to right button mouse mode, left button is already released 
	        	if(MouseMode==1)
	        	{
	        		MouseMode=3;
		        	libSDLproxy.libSDLnotifytouch(1,MouseMode,-1,-1);
	        	} else
	        	{
	        		MouseMode=1;
		        	libSDLproxy.libSDLnotifytouch(1,MouseMode,-1,-1);
	        	}
	        	break;
	        default:
	        	// 2 fingers, move one - simulate move
	        	if(libSDLconfig.CONF_TOUCH_AS_TRACKPAD)	
	        	{
		        	libSDLproxy.libSDLnotifytrackpad(0,-1,(int)scrX-TP_DeltaX,(int)scrY-TP_DeltaY);
	        		TP_DeltaX=(int)scrX;
	        		TP_DeltaY=(int)scrY;
	        	}
	        	else
	        		libSDLproxy.libSDLnotifytouch(0,-1,(int)scrX,(int)scrY);
	        	break;
	        };
        }
  
		return super.onTouchEvent(event);
	}

	@Override
	public boolean onTrackballEvent(MotionEvent event) {
//    	Log.v("Trackball", event.toString());
    	if(libSDLconfig.CONF_TRACKBALL_AS_MOUSE)
    	{
    		double TBc=libSDLconfig.CONF_TRACKBALL_MULTIPLY*TrackballAcceleration;
    		
	        switch(event.getAction())
	        {
	        case MotionEvent.ACTION_DOWN:
//	        	libSDLproxy.libSDLnotifytrackpad(0,-1,(int)(TBc*event.getX()),(int)(TBc*event.getY()),SDLview.getWidth(),SDLview.getHeight());
	        	libSDLproxy.libSDLnotifytrackpad(1,MouseMode,-1,-1);
                TrackballKillMoves = true;
	        	break;
	        case MotionEvent.ACTION_UP:
//	        	libSDLproxy.libSDLnotifytrackpad(0,-1,(int)(TBc*event.getX()),(int)(TBc*event.getY()),SDLview.getWidth(),SDLview.getHeight());
	        	libSDLproxy.libSDLnotifytrackpad(2,MouseMode,-1,-1);
                TrackballKillMoves = false;
	        	MouseMode=1;	// restore LB mode
	        	break;
	        default:
                if(!TrackballKillMoves)
	        	  libSDLproxy.libSDLnotifytrackpad(0,-1,(int)(TBc*event.getX()),(int)(TBc*event.getY()));
	        	break;
	        };
    	}

		super.onTrackballEvent(event);
		return libSDLconfig.CONF_TRACKBALL_AS_MOUSE;	// if we simulate mouse - process the message
	}

	public void CreateNotification()
	{
	    mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        Intent intent = new Intent(this, libSDL.class); 
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, 
        		Intent.FLAG_ACTIVITY_NEW_TASK);
        Notification n = new Notification(R.drawable.icon, "SDL application paused", 
        		System.currentTimeMillis());
		n.setLatestEventInfo(this, "SDL application", "Application is paused, click to activate", pendingIntent);
        mNotificationManager.notify(
                NOTIFY_ID,
                n);  
	}
}
