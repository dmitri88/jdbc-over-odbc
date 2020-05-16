package org.dmitri.jodbc;

import java.lang.reflect.InvocationTargetException;

import org.junit.jupiter.api.Test;

public class JniEntrypointTest {
	
	@Test
	public void testConnect() throws SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
		JniEntrypoint instance = JniEntrypoint.getInstance();
		instance.setConnectionParameter("Debug", "trace");
		instance.setConnectionParameter("DriverClass", "org.h2.Driver");
		instance.setConnectionParameter("URL", "jdbc:h2:mem:");
		instance.connect();
	}

}
