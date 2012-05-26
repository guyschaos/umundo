package org.umundo.s11n;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;

import org.umundo.core.Message;
import org.umundo.core.Receiver;
import org.umundo.core.Subscriber;

import com.google.protobuf.GeneratedMessage;

public class TypedSubscriber extends Subscriber {

	class DeserializingReceiverDecorator extends Receiver {
		ITypedReceiver r;

		DeserializingReceiverDecorator(ITypedReceiver r) {
			this.r = r;
		}

		public void receive(Message msg) {
			String type = msg.getMeta("type");
			byte[] data = msg.getData();
			if (TypedSubscriber.this.autoRegisterTypes && !TypedSubscriber.this.autoDeserLoadFailed.containsKey(type) && !TypedSubscriber.this.deserializerMethods.containsKey(type)) {
				try {
					Class c = Class.forName(type);
					if (GeneratedMessage.class.isAssignableFrom(c)) {
						TypedSubscriber.this.registerType((Class<? extends GeneratedMessage>) Class.forName(type));
					} 
				} catch (ClassNotFoundException e) {
					TypedSubscriber.this.autoDeserLoadFailed.put(type, null);
					TypedSubscriber.this.deserializerMethods.remove(type);
				} catch (SecurityException e) {
					TypedSubscriber.this.autoDeserLoadFailed.put(type, null);
					TypedSubscriber.this.deserializerMethods.remove(type);
				}
			}
			
			if (TypedSubscriber.this.deserializerMethods.containsKey(type)) {
				Object o = null;
				Method m = TypedSubscriber.this.deserializerMethods.get(type);
				if (m == null) {
					r.receiveObject(null, msg);
					return;
				}
				try {
					o = m.invoke(null, data);
				} catch (IllegalArgumentException e) {
					r.receiveObject(null, msg);
					return;
				} catch (IllegalAccessException e) {
					r.receiveObject(null, msg);
					return;
				} catch (InvocationTargetException e) {
					System.out.println("InvocationTargetException");
					e.getCause().printStackTrace();
					r.receiveObject(null, msg);
					return;
				}
				r.receiveObject(o, msg);
			} else {
				r.receiveObject(null, msg);
			}
		}
	}

	/**
	 * Experimental. 
	 * 
	 * @param auto
	 */
	public void setAutoRegisterTypesByReflection(boolean auto) {
		this.autoRegisterTypes = auto;
	}
	
	public void registerType(Class<? extends GeneratedMessage> type) throws SecurityException {
		String n = type.getSimpleName();
		try {
			Method m = type.getMethod("parseFrom", byte[].class);
			deserializerMethods.put(n, m);
		} catch (NoSuchMethodException e) {
			System.err.println("GeneratedMessage in protobuf no longer has a parseFrom method?");
			e.printStackTrace();
		}
	}
	
	
	public TypedSubscriber(String channel, ITypedReceiver receiver) {
		this(channel, receiver, false);
	}
	
	public TypedSubscriber(String channel, ITypedReceiver receiver, boolean autoRegisterTypes) {
		super(channel);
		decoratedReceiver = new DeserializingReceiverDecorator(receiver);
		setReceiver(decoratedReceiver);
		this.autoRegisterTypes = autoRegisterTypes;
	}

	
	private Map<String, Method> deserializerMethods = new HashMap<String, Method>();
	private DeserializingReceiverDecorator decoratedReceiver;

	private boolean autoRegisterTypes = false;
	private Map<String, Object> autoDeserLoadFailed = new HashMap<String, Object>();

}
