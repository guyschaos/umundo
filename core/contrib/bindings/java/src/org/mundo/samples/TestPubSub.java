package org.mundo.samples;

import java.lang.reflect.Field;

import org.umundo.core.Message;
import org.umundo.core.Node;
import org.umundo.core.Publisher;
import org.umundo.core.Receiver;
import org.umundo.core.Subscriber;

public class TestPubSub extends Receiver {
	
	/**
	 * We are a receiver.
	 */
	@Override
	public void receive(Message msg) {
		System.out.println("Received " + msg.getData().length() + " bytes: " + msg.getData());
		for (String key : msg.getMeta().keySet()) {
			System.out.println(key + ": " + msg.getMeta(key));
		}
	}

	public static void main(String[] args) throws Exception {
		// prepare library path and load jni library
		loadUMundoNative();
		
		Subscriber fooSub = new Subscriber("fooChannel", new TestPubSub());
		Publisher fooPub = new Publisher("fooChannel");
		Node mainNode = new Node("someDomain");		
		Node otherNode = new Node("someDomain");
		
		mainNode.addPublisher(fooPub);
		otherNode.addSubscriber(fooSub);
		
		byte[] data = new byte[64];
		for(int i = 0; i < 64; i++) {
			// visible ascii characters
			data[i] = (byte) (i%127 + 48);
		}
		int counter = 0;
		String foo = new String(data);
		
		while (true) {
			Message msg = new Message(foo);
			msg.setMeta("type", "awesome!");
			msg.setMeta("foo", "bar!");
			msg.setMeta("count", Integer.toString(counter++));

			fooPub.send(msg);
			Thread.sleep(200);
		}
	}

	/**
	 * Programmatically set the library path.
	 * 
	 * In a real application you would use -Djava.library.path="..."
	 */
	public static void loadUMundoNative() throws Exception {
		// force reread of the library path
		System.setProperty("java.library.path", "../../../../lib/darwin-i386/gnu/Release/");
		try {
			Field fieldSysPath = ClassLoader.class.getDeclaredField("sys_paths");
			fieldSysPath.setAccessible(true);
			fieldSysPath.set(null, null);
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		}
		System.loadLibrary("umundocoreSwig");
	}
}
