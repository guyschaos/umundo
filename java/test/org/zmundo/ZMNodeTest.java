/*
 * Build by TU Darmstadt 2011, all rights reserved.
 */
package org.zmundo;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.zmundo.ZMNode.State;
import static org.junit.Assert.*;

/**
 * Unit test cases for the {@link ZMNode} class. Node that it is hard to test
 * a singleton because of state creep between tests. We will do our best.
 * 
 * @author Felix Heinrichs <felix.heinrichs@cs.tu-darmstadt.de>
 */
public class ZMNodeTest {
	
	@BeforeClass
	public static void setUpClass() throws Exception {
	}

	@AfterClass
	public static void tearDownClass() throws Exception {
	}
	
	@Before
	public void setUp() {
	}
	
	@After
	public void tearDown() {
	}

	@Test
	public void testLifecycleNode() {
		assertTrue(ZMNode.State.UNINITIALIZED == ZMNode.getInstance().getState());
		assertFalse(ZMNode.getInstance().isInitialized());

		ZMNode.initNode();
		assertTrue(ZMNode.getInstance().isInitialized());
		ZMNode.shutdownNode();
		assertFalse(ZMNode.getInstance().isInitialized());
	}

	@Test(expected = IllegalStateException.class)
	public void testDoubleInitNode() {
		ZMNode.initNode();
		ZMNode.initNode();
	}

	@Test
	public void testDoubleShutdownNode() {
		ZMNode.shutdownNode();
		ZMNode.shutdownNode();
		assertTrue(ZMNode.getInstance().getState() == ZMNode.State.UNINITIALIZED);
	}

	@Test
	public void testConfiguration() {
		// 2DO write a test for checking against proper configuration
	}

	@Test
	public void testManage() {
		// 2DO write a test checking the management lifecacly
	}

	@Test
	public void testUnmanage() {
		// 2DO write a test checking the management lifecacly
	}
	
	// INNER TYPES

	/**
	 * Dummy to test the {@link IManageable} interface.
	 */
	public static class ManageableDummy implements IManageable {

		protected ZMNode.State state;

		@Override
		public void init() {
			state = ZMNode.State.INITIALIZED;
		}

		@Override
		public void shutdown() {
			state = ZMNode.State.UNINITIALIZED;
		}

		public boolean isInitialized() {return state == ZMNode.State.INITIALIZED;}
		public boolean isUninitialized() {return state == ZMNode.State.UNINITIALIZED;}
	}
}
