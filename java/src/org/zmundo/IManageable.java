/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */
package org.zmundo;

/**
 * Interface for manageable artifacts. This interface defines lifecycle methods
 * for manageable components. A component is considered manageable if it can be
 * initialized ({@link IManageable#init()}) in order to start the activity of
 * this object and shut down ({@link IManageable#shutdown()}) in order to stop
 * the activity of this object.
 * 
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 * @version 0.1
 */
public interface IManageable {

	/**
	 * Lifecycle method to start activity of an object.
	 */
	public void init();

	/**
	 * Lifecylce method to stop activity of an object.
	 */
	public void shutdown();
}
