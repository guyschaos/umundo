/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */
package org.zmundo;

import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 2DO ZMNode class doc
 * 
 * {\section Manageables}
 * 
 * The ZMNode also takes care of managing the lifecycle of certain objects in
 * the current runtime. Manageable objects need to implement the 
 * {@link IManageable} interface in order to be part of this mechanism. For
 * example, to register a certain object {@p foo} to be managed by the current
 * {@link ZMNode}, the user should call
 * {@code}
 *     ZMNode.getInstance().manage(foo);
 * {@endcode}
 * This will call the {@link IManageable#init()} method of this object upon 
 * initialization and the {@link IManageable#shutdown()} method upon shutdown.
 * The exact semantics depend on the initalization state of the node, i.e. 
 * objects that become managed only after initialization are immediately 
 * initialized, while objects that become managed before initialization will
 * only be initialized when the node is initialized. The same holds for
 * shutdown. All lifecycle methods are synchronized.
 *
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 * @version 0.1
 */
public class ZMNode {

	// SINGLETON

	private static ZMNode _instance;

	/**
	 * Obtain the {@link ZMNode} singleton.
	 * 
	 * @return 
	 */
	public static ZMNode getInstance() {
		return (_instance == null) ? (_instance = new ZMNode()):  _instance;
	}
	
	private ZMNode() {
		this.state = State.UNINITIALIZED;
		this.manageables = new CopyOnWriteArrayList<IManageable>();
		// 2DO implement the ZMNode constructor
	}

	// STATIC PART

	/**
	 * Method called to initialize the node.
	 */
	public static void initNode() {
		getInstance().init();
	}

	/**
	 * Method called to shut down the node.
	 */
	public static void shutdownNode() {
		getInstance().shutdown();
	}
	
	// MEMBERS

	private final Object lock = new Object();
	
	private ZMConfiguration configuration;
	private State state;
	private CopyOnWriteArrayList<IManageable> manageables;


    // GETTERS AND SETTERS
	
	/**
	 * Get the value of configuration
	 *
	 * @return the value of configuration
	 */
	public ZMConfiguration getConfiguration() {
		return configuration;
	}

	/**
	 * Get the value of state
	 *
	 * @return the value of state
	 */
	public State getState() {
		return state;
	}

	/**
	 * Set the value of configuration
	 *
	 * @param configuration new value of configuration
	 */
	public void setConfiguration(ZMConfiguration configuration) {
		this.configuration = configuration;
	}

	
	// METHODS

	/**
	 * Helper method to init all manageables.
	 */
	private void initManageables() {
		for (IManageable m : this.manageables) m.init();
	}

	/**
	 * Helper method to shutdown all manageables.
	 */
	private void shutdownManageables() {
		for (IManageable m : this.manageables) m.shutdown();
	}

	/**
	 * Convenience method to check whether this {@link ZMNode} is already 
	 * initialized.
	 * 
	 * @return <code>true</code> iff this node is initialized
	 */
	public boolean isInitialized() {
		return this.state == State.INITIALIZED;
	}

	/**
	 * Manage a new {@link IManageable} object.
	 * 
	 * @param e	the object to manage
	 * @return <code>true</code> iff this object can be managed
	 */
	public boolean manage(IManageable e) {
		synchronized (lock) {
			if (this.state != State.UNINITIALIZED) e.init();
			return manageables.add(e);
		}
	}

	/**
	 * Unmanage a given {@link IManageable} object.
	 * 
	 * @param e the object not to manage anymore
	 * @return <code>true</code> iff the object can be unmanaged
	 */
	public boolean unmanage(IManageable e) {
		synchronized (lock) {
			if (this.state != State.UNINITIALIZED) e.shutdown();
			return manageables.remove(e);
		}
	}

	/**
	 * Called to initialze this node.
	 */
	public void init() {
		if (this.state == State.INITIALIZED || this.state == State.INITIALIZING)
			throw new IllegalStateException("cannot initialize already "
					+ "initialized node");
		this.state = State.INITIALIZING;
		// 2DO implement initialization of the node
		this.initManageables();
		this.state = State.INITIALIZED;
	}

	/**
	 * Called to shut this node down.
	 */
	public void shutdown() {
		if (this.state == State.INITIALIZING)
			throw new IllegalStateException("cannot shutdown initializing node");
		if (this.state == State.UNINITIALIZED) return;

		// 2DO implement shutting down of the node
		this.shutdownManageables();
		this.state = State.UNINITIALIZED;
	}
	
	// INTERNAL TYPES

	/**
	 * Enum encapsulating the current state of the node.
	 */
	public static enum State {
		UNINITIALIZED, INITIALIZING, INITIALIZED;
	}
}
