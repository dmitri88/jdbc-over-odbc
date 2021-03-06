package org.dmitri.jodbc;

import java.lang.reflect.InvocationTargetException;

import org.dmitri.jodbc.enums.OdbcColumnAttribute;
import org.junit.jupiter.api.Test;

public class JniEntrypointTest {
	
	@Test
	public void testConnect() throws SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
		JniEntrypoint instance = JniEntrypoint.getInstance();
		instance.setConnectionParameter("Debug", "trace");
		instance.setConnectionParameter("DriverClass", "org.h2.Driver");
		instance.setConnectionParameter("URL", "jdbc:h2:mem:");
		instance.connect("DSN=test;");
	}
	
	@Test
	public void testDataType() throws SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
		JniEntrypoint instance = JniEntrypoint.getInstance();
		instance.setConnectionParameter("Debug", "trace");
		instance.setConnectionParameter("DriverClass", "org.h2.Driver");
		instance.setConnectionParameter("URL", "jdbc:h2:mem:");
		instance.connect("DSN=test;");
		
		long stmtId = 0x100;
		instance.createStatement(stmtId);
		instance.execDirect(stmtId, "select * from INFORMATION_SCHEMA.SESSIONS");
		
		instance.getColumnAttribute(stmtId, 1, OdbcColumnAttribute.SQL_COLUMN_UNSIGNED.getType());
		instance.getColumnAttribute(stmtId, 1, OdbcColumnAttribute.SQL_COLUMN_LENGTH.getType());
		instance.getColumnAttribute(stmtId, 2, OdbcColumnAttribute.SQL_COLUMN_LENGTH.getType());
	}

}
