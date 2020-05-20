package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcStatus {
	SQL_NULL_DATA(-1),
	SQL_DATA_AT_EXEC(-2),
	SQL_SUCCESS(0),
	SQL_SUCCESS_WITH_INFO      (1),
	//#if (ODBCVER >= 0x0300)
	SQL_NO_DATA              (100),
	//#endif
	SQL_ERROR                 (-1),
	SQL_INVALID_HANDLE        (-2),
	SQL_STILL_EXECUTING        (2),
	SQL_NEED_DATA             (99),

	//#if (ODBCVER >= 0x0380)
	SQL_PARAM_DATA_AVAILABLE    (101);  
	
	@Getter
	private int type;
	
	private OdbcStatus(int type) {
		this.type = type;
	}
	public static OdbcStatus valueOf(int e) {
		for(OdbcStatus attr:OdbcStatus.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}
}
