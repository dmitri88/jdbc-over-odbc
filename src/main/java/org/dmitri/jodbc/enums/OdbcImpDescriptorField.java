package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcImpDescriptorField {

	SQL_DESC_ARRAY_SIZE						(20),
	SQL_DESC_ARRAY_STATUS_PTR				(21),
	SQL_DESC_AUTO_UNIQUE_VALUE				(11), //SQL_COLUMN_AUTO_INCREMENT
	SQL_DESC_BASE_COLUMN_NAME				(22),
	SQL_DESC_BASE_TABLE_NAME				(23),
	SQL_DESC_BIND_OFFSET_PTR				(24),
	SQL_DESC_BIND_TYPE						(25),
	SQL_DESC_CASE_SENSITIVE					 (12), //SQL_COLUMN_CASE_SENSITIVE
	SQL_DESC_CATALOG_NAME					(17),  //SQL_COLUMN_QUALIFIER_NAME
	SQL_DESC_CONCISE_TYPE					(2), //SQL_COLUMN_TYPE
	SQL_DESC_DATETIME_INTERVAL_PRECISION	(26),
	SQL_DESC_DISPLAY_SIZE					(6), //SQL_COLUMN_DISPLAY_SIZE
	SQL_DESC_FIXED_PREC_SCALE				(9), //SQL_COLUMN_MONEY
	SQL_DESC_LABEL							(18),  //SQL_COLUMN_LABEL
	SQL_DESC_LITERAL_PREFIX					(27),
	SQL_DESC_LITERAL_SUFFIX					(28),
	SQL_DESC_LOCAL_TYPE_NAME				(29),
	SQL_DESC_MAXIMUM_SCALE					(30),
	SQL_DESC_MINIMUM_SCALE					(31),
	SQL_DESC_NUM_PREC_RADIX					(32),
	SQL_DESC_PARAMETER_TYPE					(33),
	SQL_DESC_ROWS_PROCESSED_PTR				(34),
	//#if (ODBCVER >= 0x0350)
	SQL_DESC_ROWVER							(35),
	//#endif /* ODBCVER >= 0x0350 */
	SQL_DESC_SCHEMA_NAME					(16), //SQL_COLUMN_OWNER_NAME
	SQL_DESC_SEARCHABLE						(13), //SQL_COLUMN_SEARCHABLE
	SQL_DESC_TYPE_NAME						(14),//SQL_COLUMN_TYPE_NAME
	SQL_DESC_TABLE_NAME						(15), //SQL_COLUMN_TABLE_NAME
	SQL_DESC_UNSIGNED						(8), //SQL_COLUMN_UNSIGNED
	SQL_DESC_UPDATABLE						(10), //SQL_COLUMN_UPDATABLE
	//#endif /* ODBCVER >= 0x0300 *

	SQL_DESC_COUNT                  (1001),
	SQL_DESC_TYPE                   (1002),
	SQL_DESC_LENGTH                 (1003),
	SQL_DESC_OCTET_LENGTH_PTR       (1004),
	SQL_DESC_PRECISION              (1005),
	SQL_DESC_SCALE                  (1006),
	SQL_DESC_DATETIME_INTERVAL_CODE (1007),
	SQL_DESC_NULLABLE               (1008),
	SQL_DESC_INDICATOR_PTR          (1009),
	SQL_DESC_DATA_PTR               (1010),
	SQL_DESC_NAME                   (1011),
	SQL_DESC_UNNAMED                (1012),
	SQL_DESC_OCTET_LENGTH           (1013),
	SQL_DESC_ALLOC_TYPE             (1099);


	@Getter
	private int type;
	
	private OdbcImpDescriptorField(int type) {
		this.type = type;
	}
	public static OdbcImpDescriptorField valueOf(int e) {
		for(OdbcImpDescriptorField attr:OdbcImpDescriptorField.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}
}
