package org.umundo.s11n;

import org.umundo.core.Message;

public interface ITypedReceiver {

	/** The implementation should cast the received object to a more specific type. This can be done by inspecting the runtime type, or based on information in the original msg object 
	 * 
	 * @param object the deserialized object contained in the message. Needs to be cast to its actual type to do something.
	 * @param msg the original message, mainly to access meta information
	 */
	public void receiveObject(Object object, Message msg);
}
