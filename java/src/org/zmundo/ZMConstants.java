/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */

package org.zmundo;

/**
 * Set of constants defined in zMundo. This class represents a set of constants
 * that have been defined in zMundo. Included are so called magic numbers,
 * which are e.g. used to inform the format of the on-wire reprentatin of
 * received messages.
 *
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 * @version 0.1
 */
public class ZMConstants {

	/**
	 * Byte pattern indicating a binary ZMQ topic: the payload of this message
	 * is only a length encoded byte array.
	 */
	public static final byte ZM_BLOB = 0x00;

	/**
	 * Byte pattern indicating a {@link ZMMessage} ZMQ topic: the payload of
	 * this message is a serialized {@link ZMMessage}.
	 */
	public static final byte ZM_MESSAGE = 0x01;

	/**
	 * Byte pattern indicating a {@link ZMRemoteMethodCall} ZMQ topic: the
	 * payload of this message is a serialized remote method call.
	 */
	public static final byte ZM_RMC = 0x02;

}
