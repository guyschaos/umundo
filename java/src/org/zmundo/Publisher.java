/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */

package org.zmundo;

import org.zeromq.ZMQ.Socket;
import org.zmundo.msg.ZMMessageProtos.ZMMessage;

/**
 *
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 */
public class Publisher {

	protected Socket pubSocket;

	/**
	 * <i>Lifecycle Method</i>: Called in order to make the publisher available
	 * for subscriptions in the zmundo overlay (or locally).
	 * 
	 * @param tm the {@link TopicManager} to use for advertisement
	 */
	protected void advertise(TopicManager tm) {
		// 2DO implement
	}

	/**
	 * <i>Lifecycle Method</i>: Called to declare that this publisher is not
	 * available for subscriptions anymore.
	 *
	 * @param tm the {@link TopicManager} to use for de-advertisement
	 */
	protected void deadvertise(TopicManager tm) {
		// 2DO implement
	}

	/**
	 * Method to publish a {@link ZMMessage} on the topic associated with
	 * this publisher.
	 *
	 * @param message
	 */
	public void publish(ZMMessage message) {
		
	}
}
