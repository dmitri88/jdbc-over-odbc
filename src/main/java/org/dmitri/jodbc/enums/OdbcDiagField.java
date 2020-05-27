package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcDiagField {
	SQL_DIAG_CURSOR_ROW_COUNT			(-1249),
	SQL_DIAG_ROW_NUMBER					(-1248),
	SQL_DIAG_COLUMN_NUMBER				(-1247)	;
	
	@Getter
	private int type;
	
	private OdbcDiagField(int type) {
		this.type = type;
	}
	public static OdbcDiagField valueOf(int e) {
		for(OdbcDiagField attr:OdbcDiagField.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}		
}
