/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */
package org.zmundo;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import org.zmundo.tb.CentralTopicBroker;
import org.zmundo.tb.ITopicBroker;

/**
 * Central options container representing options set to configure zMundo. This
 * class provides a central options container for all general options that
 * configure a {@link ZMNode}.
 * 
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 * @version 0.1
 */
public class ZMConfiguration {
	
	// DEFAULTS
	
	/**
	 * The {@link CentralTopicBroker} class.
	 */
	public static final String CENTRAL_TB = CentralTopicBroker.class.getName();
	
	/**
	 * Descriptor for custom configurations.
	 */
	public static final String CUSTOM_DESCRIPTOR = "CUSTOM";
	public static final String CTB_DESCRIPTOR = "CENTRAL TB";
			
	// PROPERTIES
	
	private String topicBroker;
	public static final String PROP_TOPICBROKER = "topicBroker";

	/**
	 * Get the value of topicBroker. The value of topic broker is the class
	 * name of a topic broker class implementing {@link ITopicBroker}.
	 *
	 * @return the value of topicBroker
	 */
	public String getTopicBroker() {
		return topicBroker;
	}

	/**
	 * Set the value of topicBroker. The value of topic broker to be a class
	 * name of a topic broker class implementing {@link ITopicBroker}.
	 *
	 * @param topicBroker new value of topicBroker
	 */
	public void setTopicBroker(String topicBroker) {
		String oldTopicBroker = this.topicBroker;
		this.topicBroker = topicBroker;
		propertyChangeSupport.firePropertyChange(PROP_TOPICBROKER, oldTopicBroker, topicBroker);
	}

	private PropertyChangeSupport propertyChangeSupport = new PropertyChangeSupport(this);

	/**
	 * Add PropertyChangeListener.
	 *
	 * @param listener
	 */
	public void addPropertyChangeListener(PropertyChangeListener listener) {
		propertyChangeSupport.addPropertyChangeListener(listener);
	}

	/**
	 * Remove PropertyChangeListener.
	 *
	 * @param listener
	 */
	public void removePropertyChangeListener(PropertyChangeListener listener) {
		propertyChangeSupport.removePropertyChangeListener(listener);
	}

	private String descriptor;

	/**
	 * Get the value of descriptor. A descriptor is a human readable 
	 * representation of this particular option configuration.
	 *
	 * @return the value of descriptor
	 */
	public String getDescriptor() {
		return descriptor;
	}

	/**
	 * Set the value of descriptor. A descriptor is a human readable 
	 * representation of this particular option configuration.
	 *
	 * @param descriptor new value of descriptor
	 */
	public void setDescriptor(String descriptor) {
		this.descriptor = descriptor;
	}

}
