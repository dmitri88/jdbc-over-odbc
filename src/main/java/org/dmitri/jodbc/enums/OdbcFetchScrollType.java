package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcFetchScrollType {
	SQL_FETCH_NEXT      (1),
	SQL_FETCH_FIRST     (2),

	/* Other codes used for FetchOrientation in SQLFetchScroll() */
	SQL_FETCH_LAST      (3),
	SQL_FETCH_PRIOR     (4),
	SQL_FETCH_ABSOLUTE  (5),
	SQL_FETCH_RELATIVE  (6);
	
	@Getter
	private int type;
	
	private OdbcFetchScrollType(int type) {
		this.type = type;
	}
	public static OdbcFetchScrollType valueOf(int e) {
		for(OdbcFetchScrollType attr:OdbcFetchScrollType.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}	
}
