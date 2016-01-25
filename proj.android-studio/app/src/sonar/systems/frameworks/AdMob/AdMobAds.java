package sonar.systems.frameworks.AdMob;


import sonar.systems.frameworks.BaseClass.Framework;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;


public class AdMobAds extends Framework
{
	private Activity activity;
	
	private String banner_id_top = "";
	private String banner_id_bottom = "";
	private String interstitial_id = "";
	private String test_device_id = "";
	
	private static AdView top_banner_adView;
	private static AdView bottom_banner_adView;
	private InterstitialAd interstitial;

	FrameLayout.LayoutParams top_adParams;
	FrameLayout.LayoutParams bottom_adParams;
	
	private final int BOTTOM = 0;
	private final int TOP = 1;
	private final int BOTH = 2;
	
	public static native void FullscreenAdPreloaded(boolean result);
	
	private boolean preLoadCalled = false;
	
	public AdMobAds()
	{
		
	}
	
	
	@Override
	public void SetActivity(Activity activity)
	{
		this.activity = activity;
		
		test_device_id = activity.getResources().getString(activity.getResources().getIdentifier("admob_test_device_id","string",activity.getPackageName()));
		banner_id_top = activity.getResources().getString(activity.getResources().getIdentifier("admob_banner_top_id","string",activity.getPackageName()));
		banner_id_bottom = activity.getResources().getString(activity.getResources().getIdentifier("admob_banner_bottom_id","string",activity.getPackageName()));
		interstitial_id = activity.getResources().getString(activity.getResources().getIdentifier("admob_interstitial_id","string",activity.getPackageName()));
	}
	
	
	@Override
	public void onCreate(Bundle b) 
	{
        Log.d("lukasz", "hallo ad mob!");
		//Create top banner
		top_banner_adView = new AdView(activity);
		top_banner_adView.setAdSize(AdSize.BANNER);
		top_banner_adView.setAdUnitId(banner_id_top);

		AdRequest adRequest = new AdRequest.Builder()
		.addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
		.addTestDevice("HASH_DEVICE_ID")
		.addTestDevice(test_device_id)
		.build();
		
		top_adParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT, android.view.Gravity.TOP|android.view.Gravity.CENTER_HORIZONTAL);

		top_banner_adView.loadAd(adRequest);
		top_banner_adView.setBackgroundColor(Color.BLACK);
		top_banner_adView.setBackgroundColor(0);
		
		activity.addContentView(top_banner_adView,top_adParams);
		
		//Create Bottom Banner
		bottom_banner_adView = new AdView(activity);
		bottom_banner_adView.setAdSize(AdSize.BANNER);
		bottom_banner_adView.setAdUnitId(banner_id_bottom);

		AdRequest adRequest1 = new AdRequest.Builder()
		.addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
		.addTestDevice("HASH_DEVICE_ID")
		.addTestDevice(test_device_id)
        .addTestDevice("860F629E58454CD81E18A30516950FA7")
		.build();
		
		bottom_adParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT, android.view.Gravity.BOTTOM|android.view.Gravity.CENTER_HORIZONTAL);

		bottom_banner_adView.loadAd(adRequest1);
		bottom_banner_adView.setBackgroundColor(Color.BLACK);
		bottom_banner_adView.setBackgroundColor(0);
		
		activity.addContentView(bottom_banner_adView, bottom_adParams);

		// Create the interstitial.
	    interstitial = new InterstitialAd(activity);
	    interstitial.setAdUnitId(interstitial_id);

	    interstitial.setAdListener(new AdListener()
	    {
	          public void onAdLoaded()
	          {
	        	  if(preLoadCalled)
	        	  {
	        		  FullscreenAdPreloaded(true);
	        	  }
	        	  else
	        	  {
	        		  LoadFullscreenAd();
	        	  }
	        	  
	        	  preLoadCalled = false;
	        	  
	          }
	    });
	    
	    HideBannerAd(BOTH);
	}
	
	@Override
	public void onStart() 
	{

	}

	@Override
	public void onStop() 
	{

	}

	@Override
	public void onPause()
	{
		if (top_banner_adView != null) 
		{
			top_banner_adView.pause();
		}
		
		if (bottom_banner_adView != null) 
		{
			bottom_banner_adView.pause();
		}
	}
	
	@Override
	public void onActivityResult(int request, int response, Intent data)
	{

	}
	
	@Override
	public void onResume()
	{
		if (top_banner_adView != null) 
		{
			top_banner_adView.resume();
		}
		
		if (bottom_banner_adView != null) 
		{
			bottom_banner_adView.resume();
		}
	}
	
	@Override
	public void onDestroy() 
	{
		top_banner_adView.destroy();
		bottom_banner_adView.destroy();
	}
	
	@Override
	public void ShowBannerAd()
	{
        Log.d("lukasz", "ad mob show banner!");
		if (top_banner_adView != null) 
		{
		 activity.runOnUiThread(new Runnable()
	     {

		     @Override
		     public void run()
		     {  
		    	 	if (!top_banner_adView.isEnabled())
		    	 		top_banner_adView.setEnabled(true);
		    	 	 if (top_banner_adView.getVisibility() == 4 )
		    	 		top_banner_adView.setVisibility(View.VISIBLE); 
		     }
	     });
		}

	  
	}
	
	@Override
	public void ShowBannerAd(final int position)
	{
        Log.d("lukasz", "ad mob show banner position:" + position);

		switch(position)
		{
			case TOP:
			{
				if (top_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     { 	
				    	 if (!top_banner_adView.isEnabled())
				    	 	 top_banner_adView.setEnabled(true);
				    	 
				    	 if (top_banner_adView.getVisibility() == View.INVISIBLE )
				    	 	 top_banner_adView.setVisibility(View.VISIBLE); 
				     }
			     });
				}
				break;
			}
			case BOTTOM:
			{
                Log.d("lukasz", "showing bottom, bottom_banner_adView: " + bottom_banner_adView + ", bottom_banner_adView.isEnabled(): " + bottom_banner_adView.isEnabled());
				if (bottom_banner_adView != null) 
				{
                Log.d("lukasz", "showing bottom, new runnable");
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     { 	

                     Log.d("lukasz", "showing bottom, bottom_banner_adView: " + bottom_banner_adView +
                     ", bottom_banner_adView.isEnabled(): " + bottom_banner_adView.isEnabled() +
                     ", bottom_banner_adView.getVisibility(): " + bottom_banner_adView.getVisibility());

				    	 if (!bottom_banner_adView.isEnabled())
				    		 bottom_banner_adView.setEnabled(true);
				    	 
				    	 if (bottom_banner_adView.getVisibility() == View.INVISIBLE )
				    		 bottom_banner_adView.setVisibility(View.VISIBLE); 

                         bottom_banner_adView.setVisibility(View.VISIBLE); 
				     }
			     });
				}
				break;
			}
			case BOTH:
			{
				if (top_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     { 	
				    	 if (!top_banner_adView.isEnabled())
				    	 	 top_banner_adView.setEnabled(true);
				    	 
				    	 if (top_banner_adView.getVisibility() == View.INVISIBLE )
				    	 	 top_banner_adView.setVisibility(View.VISIBLE); 
				     }
			     });
				}
				
				if (bottom_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     { 	
				    	 if (!bottom_banner_adView.isEnabled())
				    		 bottom_banner_adView.setEnabled(true);
				    	 
				    	 if (bottom_banner_adView.getVisibility() == View.INVISIBLE )
				    		 bottom_banner_adView.setVisibility(View.VISIBLE); 
				     }
			     });
				}
				break;
			}
		}
		
		

	  
	}

	@Override
	public void HideBannerAd()
	{
		if (top_banner_adView != null) 
		{
		 activity.runOnUiThread(new Runnable()
	     {

		     @Override
		     public void run()
		     {
		    	 	if (top_banner_adView.isEnabled())
		    	 		top_banner_adView.setEnabled(false);
		    	 	if (top_banner_adView.getVisibility() != View.INVISIBLE )
		    	 		top_banner_adView.setVisibility(View.INVISIBLE);
		     }
	     });
		}
		
		if (bottom_banner_adView != null) 
		{
		 activity.runOnUiThread(new Runnable()
	     {

		     @Override
		     public void run()
		     {
		    	 	if (bottom_banner_adView.isEnabled())
		    	 		bottom_banner_adView.setEnabled(false);
		    	 	if (bottom_banner_adView.getVisibility() != View.INVISIBLE )
		    	 		bottom_banner_adView.setVisibility(View.INVISIBLE);
		     }
	     });
		}
	}
	
	@Override
	public void HideBannerAd(int position)
	{
		switch(position)
		{
			case TOP:
			{
				if (top_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     {
				    	 	if (top_banner_adView.isEnabled())
				    	 		top_banner_adView.setEnabled(false);
				    	 	if (top_banner_adView.getVisibility() != 4 )
				    	 		top_banner_adView.setVisibility(View.INVISIBLE);
				     }
			     });
				}
				break;
			}
			case BOTTOM:
			{
				if (bottom_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     {
				    	 	if (bottom_banner_adView.isEnabled())
				    	 		bottom_banner_adView.setEnabled(false);
				    	 	if (bottom_banner_adView.getVisibility() != 4 )
				    	 		bottom_banner_adView.setVisibility(View.INVISIBLE);
				     }
			     });
				}
				break;
			}
			case BOTH:
			{
				if (top_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     {
				    	 	if (top_banner_adView.isEnabled())
				    	 		top_banner_adView.setEnabled(false);
				    	 	if (top_banner_adView.getVisibility() != 4 )
				    	 		top_banner_adView.setVisibility(View.INVISIBLE);
				     }
			     });
				}
				
				if (bottom_banner_adView != null) 
				{
				 activity.runOnUiThread(new Runnable()
			     {

				     @Override
				     public void run()
				     {
				    	 	if (bottom_banner_adView.isEnabled())
				    	 		bottom_banner_adView.setEnabled(false);
				    	 	if (bottom_banner_adView.getVisibility() != 4 )
				    	 		bottom_banner_adView.setVisibility(View.INVISIBLE);
				     }
			     });
				}
				break;
			}
		}
		
	}
	
	void LoadFullscreenAd()
	{
		if(interstitial != null) 
		{
			activity.runOnUiThread(new Runnable()
		     {
	
			     @Override
			     public void run()
			     {
			    	 if(interstitial.isLoaded())
			    		 interstitial.show();
			     }
		     });
		}
	}
	
	@Override
	public void ShowFullscreenAd()
	{   
		
	    
	    if(interstitial != null) 
		{
			activity.runOnUiThread(new Runnable()
		    {
	
			     @Override
			     public void run()
			     {
			    	// Create ad request.
			 	    AdRequest adRequest = new AdRequest.Builder()
			 	    .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
			 		.addTestDevice("HASH_DEVICE_ID")
			 		.addTestDevice(test_device_id).build();

			 	    // Begin loading your interstitial.
			 	    interstitial.loadAd(adRequest);
			     }
		     });
		}
	}
	
	@Override
	public void PreLoadFullscreenAd()
	{
		if(interstitial != null) 
		{
			activity.runOnUiThread(new Runnable()
		    {
	
			     @Override
			     public void run()
			     {
			    	// Create ad request.
			 	    AdRequest adRequest = new AdRequest.Builder()
			 	    .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
			 		.addTestDevice("HASH_DEVICE_ID")
			 		.addTestDevice(test_device_id).build();

			 	    // Begin loading your interstitial.
			 	    interstitial.loadAd(adRequest);
			 	    preLoadCalled = true;
			     }
		     });
		}
	}
	
	@Override
	public void ShowPreLoadedFullscreenAd()
	{
		if(interstitial != null) 
		{
			activity.runOnUiThread(new Runnable()
		     {
	
			     @Override
			     public void run()
			     {
			    	 if(interstitial.isLoaded())
			    		 interstitial.show();
			     }
		     });
		}
	}

}
