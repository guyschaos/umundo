package org.umundo.s11n;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;

import org.umundo.core.Message;
import org.umundo.core.Publisher;

import com.google.protobuf.MessageLite;

/**
 * 
 * 
 * @author ds1019
 *
 */
public class TypedPublisher extends Publisher {

	public TypedPublisher(String channel) {
		super(channel);
	}
	
	public Message prepareMessage(MessageLite o) {
			return prepareMessage(o.getClass().getSimpleName(), o);
	}
	
  public Message prepareMessage(String type, MessageLite o) {
		Message msg = new Message();
		byte[] buffer = o.toByteArray();
		msg.setData(buffer);
		msg.setMeta("type", type);		
		return msg;
	}
	
	public void sendObject(MessageLite o) {
		sendObject(o.getClass().getName(), o);
	}
	
	public void sendObject(String type, MessageLite o) {
	  send(prepareMessage(type, o));
	}

}
