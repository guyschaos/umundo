package org.mundo.samples;

import java.lang.reflect.Field;

import org.umundo.Publisher;
import org.umundo.Receiver;
import org.umundo.Subscriber;

public class TestPubSub extends Receiver {

	@Override
	public void receive(byte[] data) {
		System.out.println("Received " + data.length + "bytes");
	}

	public static void main(String[] args) throws Exception {
		// prepare library path and load jni library
		System.setProperty("java.library.path", "../../../lib/darwin-i386/gnu");
		forceLibraryPathReload();
		System.loadLibrary("umundoSwig");

		Subscriber fooSub = new Subscriber("fooChannel", new TestPubSub());
		Publisher fooPub = new Publisher("fooChannel");
		byte buffer[] = new byte[1024];
		for (int i = 0; i < buffer.length; i++) {
			buffer[i] = (byte) (i % 255);
		}

		while (true) {
			fooPub.send(buffer);
			Thread.sleep(200);
		}
	}

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
