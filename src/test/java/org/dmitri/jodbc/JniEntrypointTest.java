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
	
	@Test
	public void testDataType() throws SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
		JniEntrypoint instance = JniEntrypoint.getInstance();
		instance.setConnectionParameter("Debug", "trace");
		instance.setConnectionParameter("DriverClass", "org.h2.Driver");
		instance.setConnectionParameter("URL", "jdbc:h2:mem:");
		instance.connect();
		
		long stmtId = 0x100;
		instance.createStatement(stmtId);
		instance.execDirect(stmtId, "select * from INFORMATION_SCHEMA.SESSIONS");
		
		instance.getColumnAttribute(stmtId, 1, 8);
	}

}
