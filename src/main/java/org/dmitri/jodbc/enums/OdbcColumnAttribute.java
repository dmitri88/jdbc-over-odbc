package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcColumnAttribute {

	SQL_COLUMN_LENGTH(3),
	SQL_COLUMN_UNSIGNED(8),
	SQL_COLUMN_UPDATABLE(10),
	SQL_COLUMN_LABEL(18);
	
	@Getter
	private int type;
	
	private OdbcColumnAttribute(int type) {
		this.type = type;
	}
	public static OdbcColumnAttribute valueOf(int e) {
		for(OdbcColumnAttribute attr:OdbcColumnAttribute.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}
	
}
