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
	}

	public static void main(String[] args) throws Exception {
		// prepare library path and load jni library
		System.setProperty("java.library.path", "../../../../lib/darwin-i386/gnu/Release/");
		forceLibraryPathReload();
		System.loadLibrary("umundocoreSwig");

		Subscriber fooSub = new Subscriber("fooChannel", new TestPubSub());
		Publisher fooPub = new Publisher("fooChannel");

		Node mainNode = new Node("someDomain");
		mainNode.addPublisher(fooPub);
		
		Node otherNode = new Node("someDomain");
		otherNode.addSubscriber(fooSub);
		
		byte[] data = new byte[1024];
		for(int i = 0; i < 1024; i++) {
			data[i] = (byte) (i%255);
		}
		String foo = new String(data);
		Message msg = new Message(foo);
		
		while (true) {
			fooPub.send(msg);
			Thread.sleep(200);
		}
	}

	/**
	 * Programmatically set the library path.
	 * 
	 * In a real application you would use -Djava.library.path="..."
	 */
	public static void forceLibraryPathReload() throws Exception {
		// force reread of the library path
		try {
			Field fieldSysPath = ClassLoader.class.getDeclaredField("sys_paths");
			fieldSysPath.setAccessible(true);
			fieldSysPath.set(null, null);
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		}
	}
}
