package org.umundo;

import android.app.Activity;
import android.os.Bundle;

public class UMundoAndroidActivity extends Activity {

    Publisher pub;
    Subscriber sub;
    Receiver recv;
    
    static {
//        System.loadLibrary("stdc++");
        System.loadLibrary("umundoSwig");
    }
    
    public class TestReceiver extends Receiver {

		@Override
		public void receive(byte[] arg0) {
			System.out.println("Android received " + arg0.length + " bytes!");
		}
    	
    }
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        System.out.println("FOO!");
        pub = new Publisher("fooChannel");
        System.out.println("BAR!");

        recv = new TestReceiver();
        sub = new Subscriber("fooChannel", recv);
    }
}