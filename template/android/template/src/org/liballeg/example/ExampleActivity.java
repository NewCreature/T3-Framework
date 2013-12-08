package org.liballeg.example;

import org.liballeg.android.AllegroActivity;
import android.net.Uri;
import android.content.Intent;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;

public class ExampleActivity extends AllegroActivity {

   /* load allegro */
	static
	{
		/* FIXME: see if we can't load the allegro library name, or type from the manifest here */
		System.loadLibrary("allegro");
		System.loadLibrary("allegro_primitives");
		System.loadLibrary("allegro_image");
		System.loadLibrary("allegro_font");
		System.loadLibrary("allegro_ttf");
		System.loadLibrary("allegro_audio");
		System.loadLibrary("allegro_acodec");
		System.loadLibrary("allegro_physfs");
		System.loadLibrary("allegro_memfile");
		//System.loadLibrary("allegro_monolith");
	}
	public ExampleActivity()
	{
		super("libAPP_NAME.so");
	}
/*	public void showSoftInput(boolean toggle)
	{
		if(toggle)
		{
			getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
		}
		else
		{
			getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
		}
	} */

	public void showSoftInput()
	{
		runOnUiThread(new Runnable() { public void run() { getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE); }});
	}
	public void OpenKeyBoard()
	{
		Context mContext = getApplicationContext();
        InputMethodManager imm = (InputMethodManager) mContext.getSystemService(mContext.INPUT_METHOD_SERVICE);
        imm.toggleSoftInput(InputMethodManager.SHOW_FORCED,0);
    }
    //For close keyboard
    public void CloseKeyBoard()
    {
		Context mContext = getApplicationContext();
        InputMethodManager imm = (InputMethodManager) mContext.getSystemService(mContext.INPUT_METHOD_SERVICE);
        imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY,0);
    }
    
	public void openURL(String url)
	{
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		startActivity(intent);
	}
}
