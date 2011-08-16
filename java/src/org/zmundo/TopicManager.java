/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */
package org.zmundo;

import org.zmundo.tb.ITopicBroker;

/**
 * Facade to the used {@link ITopicBroker}. Provides a lightweight facade to
 * the actual {@link ITopicBroker} in use (which depends on the current
 * configuration). The {@link TopicManager} knows all available topics that have
 * been advertised in its scope, that is local topics and remote topics as
 * available via the {@link ITopicBroker}.
 *
 * 2DO complete the docs for the topic manager
 *
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 * @version 0.1
 */
public class TopicManager {

	private ITopicBroker topicBroker;

	/**
	 * Get the value of topicBroker
	 *
	 * @return the value of topicBroker
	 */
	public ITopicBroker getTopicBroker() {
		return topicBroker;
	}

	/**
	 * Set the value of topicBroker
	 *
	 * @param topicBroker new value of topicBroker
	 */
	public void setTopicBroker(ITopicBroker topicBroker) {
		this.topicBroker = topicBroker;
	}

}
