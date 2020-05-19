package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcFreeStatement {
	SQL_CLOSE           (0), //Closes the cursor associated with StatementHandle 
	//SQL_DROP            (1), //This option is deprecated. A call to SQLFreeStmt with an Option of SQL_DROP is mapped in the Driver Manager 
	SQL_UNBIND          (2), //sets the SQL_DESC_COUNT field of the ARD to 0, releasing all column buffers bound by SQLBindCol
	SQL_RESET_PARAMS    (3); //Sets the SQL_DESC_COUNT field of the APD to 0, releasing all parameter buffers set by SQLBindParameter
	
	@Getter
	private int type;
	
	private OdbcFreeStatement(int type) {
		this.type = type;
	}
	public static OdbcFreeStatement valueOf(int e) {
		for(OdbcFreeStatement attr:OdbcFreeStatement.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}		
}
