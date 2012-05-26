package org.umundo.rpc;

import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import org.umundo.core.Connectable;
import org.umundo.core.Message;
import org.umundo.core.Node;
import org.umundo.core.PublisherSet;
import org.umundo.core.SubcriberSet;
import org.umundo.protobuf.tests.TestServices.EchoReply;
import org.umundo.s11n.ITypedReceiver;
import org.umundo.s11n.TypedPublisher;
import org.umundo.s11n.TypedSubscriber;

import com.google.protobuf.MessageLite;

public class ServiceStub extends Connectable implements ITypedReceiver {

	protected String _channelName;
	protected String _serviceName;
	protected TypedPublisher _rpcPub;
	protected TypedSubscriber _rpcSub;

	Map<String, Object> _requests = new HashMap<String, Object>();
	Map<String, Object> _responses = new HashMap<String, Object>();

	public ServiceStub(String channelName) {
		_channelName = channelName;
		_rpcPub = new TypedPublisher(_channelName);
		_rpcSub = new TypedSubscriber(_channelName, this);
	}

	protected ServiceStub(ServiceDescription svcDesc) {
		_channelName = svcDesc.getChannelName();
		_rpcPub = new TypedPublisher(_channelName);
		_rpcSub = new TypedSubscriber(_channelName, this);
		for (Node node : svcDesc._svcMgr.getNodes()) {
			node.addSubscriber(_rpcSub);
			node.addPublisher(_rpcPub);
		}
		_rpcPub.waitForSubscribers(1);
	}

	@Override
	public void receiveObject(Object object, Message msg) {
		if (msg.getMeta().containsKey("respId")) {
			String respId = msg.getMeta("respId");
			if (_requests.containsKey(respId)) {
				_responses.put(respId, object);
				synchronized (_requests.get(respId)) {
					_requests.get(respId).notifyAll();
				}
			}
		}
	}

	public Object callStubMethod(String name, MessageLite in, String inType, String outType) throws InterruptedException {
		Message rpcReqMsg = _rpcPub.prepareMessage(inType, in);
		String reqId = UUID.randomUUID().toString();
		rpcReqMsg.setMeta("reqId", reqId);
		rpcReqMsg.setMeta("methodName", name);
		rpcReqMsg.setMeta("outType", outType);
		_requests.put(reqId, new Object());
		_rpcPub.send(rpcReqMsg);
		synchronized (_requests.get(reqId)) {
			_requests.get(reqId).wait();
		}
		_requests.remove(reqId);
		Object out = _responses.get(reqId);
		_responses.remove(reqId);
		return out;
	}

	public String getChannelName() {
		return _channelName;
	}

	public String getName() {
		return _serviceName;
	}

	@Override
	public PublisherSet getPublishers() {
		PublisherSet pubs = new PublisherSet();
		pubs.insert(_rpcPub);
		return pubs;
	}

	@Override
	public SubcriberSet getSubscribers() {
		SubcriberSet subs = new SubcriberSet();
		subs.insert(_rpcSub);
		return subs;
	}

}
