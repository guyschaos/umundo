package org.umundo.android;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import org.umundo.core.Message;
import org.umundo.core.Node;
import org.umundo.core.Publisher;
import org.umundo.core.Receiver;
import org.umundo.core.Subscriber;

import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.MulticastLock;
import android.os.Bundle;
import android.util.Log;

public class UMundoAndroidActivity extends Activity {

	Thread testPublishing;
	Node pubNode;
	Node subNode;
	Publisher fooPub;
	Subscriber fooSub;

	public class TestPublishing implements Runnable {

		@Override
		public void run() {
			String message = "This is foo from android";
			while (testPublishing != null) {
				Log.v("android-umundo", "Publishing");
				fooPub.send(message, message.length());
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}

	public class TestReceiver extends Receiver {
		@Override
		public void receive(Message msg) {
			Log.v("android-umundo", msg.getData());
		}

	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		System.loadLibrary("mDNSEmbedded");
		System.loadLibrary("umundocoreSwig_d");
		setContentView(R.layout.main);

		// JmDNS jmdns;
		// try {
		// jmdns = JmDNS.create();
		// jmdns.registerService(ServiceInfo.create("_umundo._tcp.local.",
		// "foo._umundo._tcp.local.", 1234, 0, 0, "path=index.html"));
		// } catch (IOException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// }

		WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
		if (wifi != null) {
			MulticastLock mcLock = wifi.createMulticastLock("mylock");
			mcLock.acquire();
			Log.v("android-umundo", wifi.getDhcpInfo().toString());
			mcLock.release();
		} else {
			Log.v("android-umundo", "Cannot get WifiManager");
		}

		try {
			for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) {
				NetworkInterface intf = en.nextElement();
				for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();) {
					InetAddress inetAddress = enumIpAddr.nextElement();
					if (!inetAddress.isLoopbackAddress()) {
						Log.v("android-umundo", inetAddress.getHostAddress().toString());
					}
				}
			}
		} catch (SocketException ex) {
			Log.e("android-umundo", ex.toString());
		}

		try {
			InetAddress groupAddr = InetAddress.getByName("224.0.0.1");
			InetSocketAddress groupSockAddr = new InetSocketAddress(groupAddr, 9040);
			MulticastSocket mSock = new MulticastSocket(9040);
			mSock.joinGroup(groupSockAddr, null);
			InetAddress localAddress = mSock.getInterface();
			byte[] recvData = new byte[512];
			DatagramPacket recvDatagram = new DatagramPacket(recvData, recvData.length);
			mSock.setSoTimeout(5000);
			mSock.receive(recvDatagram);

		} catch (Exception ex) {
			Log.e("android-umundo", ex.toString());
		}

		// pubNode = new Node();
		// subNode = new Node();
		// fooPub = new Publisher("fooChannel");
		// pubNode.addPublisher(fooPub);
		// fooSub = new Subscriber("fooChannel", new TestReceiver());
		// subNode.addSubscriber(fooSub);
		// testPublishing = new Thread(new TestPublishing());
		// testPublishing.start();
	}
}