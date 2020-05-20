package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcColumnAttribute {
	SQL_COLUMN_COUNT                (0),
	SQL_COLUMN_NAME                 (1),
	SQL_COLUMN_TYPE                 (2),

	SQL_COLUMN_LENGTH(3),
	SQL_COLUMN_PRECISION            (4),
	SQL_COLUMN_SCALE                (5),
	SQL_COLUMN_DISPLAY_SIZE         (6),
	SQL_COLUMN_NULLABLE             (7),

	SQL_COLUMN_UNSIGNED(8),
	SQL_COLUMN_MONEY                (9),

	SQL_COLUMN_UPDATABLE(10),
	SQL_COLUMN_AUTO_INCREMENT(11),
	SQL_COLUMN_CASE_SENSITIVE       (12),
	SQL_COLUMN_SEARCHABLE           (13),
	SQL_COLUMN_TYPE_NAME            (14),
	SQL_COLUMN_TABLE_NAME           (15),
	SQL_COLUMN_OWNER_NAME           (16),
	SQL_COLUMN_QUALIFIER_NAME       (17),//SQL_DESC_CATALOG_NAME
	SQL_COLUMN_LABEL(18),
	SQL_DESC_ARRAY_SIZE						(20),
	SQL_DESC_ARRAY_STATUS_PTR			(	21),
	SQL_DESC_BASE_COLUMN_NAME				(22),
	SQL_DESC_BASE_TABLE_NAME				(23),
	SQL_DESC_BIND_OFFSET_PTR			(	24),
	SQL_DESC_BIND_TYPE					(	25),
	
	
	UNKNOWN_SHIT_1						(1212);
	
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
