package org.umundo.rpc;

import java.util.UUID;

import org.umundo.core.Message;
import org.umundo.s11n.ITypedReceiver;

import com.google.protobuf.MessageLite;

public abstract class Service extends ServiceStub implements ITypedReceiver {

	public Service() {
		this(UUID.randomUUID().toString());
	}
	
	private Service(ServiceDescription svcDesc) {
		super(svcDesc);
	}

	private Service(String channelName) {
		super(channelName);
	}
	
	@Override
	public void receiveObject(Object object, Message msg) {
		if (msg.getMeta().containsKey("methodName")) {
			String methodName = msg.getMeta("methodName");
			String inType = msg.getMeta("type");
			String outType = msg.getMeta("outType");
			Object out = null;
			try {
				out = callMethod(methodName, (MessageLite)object, inType, outType);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			Message rpcReplMsg = _rpcPub.prepareMessage(outType, (MessageLite) out);
			rpcReplMsg.setMeta("respId", msg.getMeta("reqId"));
			_rpcPub.send(rpcReplMsg);
		}

	}

	public abstract Object callMethod(String name, MessageLite in, String inType, String outType) throws InterruptedException;

}
