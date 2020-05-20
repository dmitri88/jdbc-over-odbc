package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcBindType {
	SQL_C_CHAR(1),
	SQL_C_LONG(4),
	SQL_C_SHORT(5),
	SQL_C_FLOAT(7),
	SQL_C_DOUBLE(8),
	SQL_C_NUMERIC(2),
	SQL_C_DEFAULT(99),
	SQL_C_WCHAR(-8),
	SQL_C_DATE(91),
	SQL_C_TIME(92),
	SQL_C_TIMESTAMP(93),
	SQL_C_INTERVAL_MONTH(102),
	SQL_C_INTERVAL_YEAR(101),
	SQL_C_INTERVAL_YEAR_TO_MONTH(107),
	SQL_C_INTERVAL_DAY(103),
	SQL_C_INTERVAL_HOUR(104),
	SQL_C_INTERVAL_MINUTE(105),
	SQL_C_INTERVAL_SECOND(106),
	SQL_C_INTERVAL_DAY_TO_HOUR(108),
	SQL_C_INTERVAL_DAY_TO_MINUTE(109),
	SQL_C_INTERVAL_DAY_TO_SECOND(110),
	SQL_C_INTERVAL_HOUR_TO_MINUTE(111),
	SQL_C_INTERVAL_HOUR_TO_SECOND(112),
	SQL_C_INTERVAL_MINUTE_TO_SECOND(113),
	SQL_C_BINARY(-2),
	SQL_C_BIT        (-7),
	SQL_C_SBIGINT	(-25),	   /* SIGNED BIGINT */
	SQL_C_UBIGINT	(-27) ,  /* UNSIGNED BIGINT */
	SQL_C_TINYINT(-6),
	SQL_C_SLONG      (-16),    /* SIGNED INTEGER  */
	SQL_C_SSHORT     (-15),   /* SIGNED SMALLINT */
	SQL_C_STINYINT   (-26),   /* SIGNED TINYINT  */
	SQL_C_ULONG      (-18),  /* UNSIGNED INTEGER*/
	SQL_C_USHORT     (-17), /* UNSIGNED SMALLINT*/
	SQL_C_UTINYINT   (-28), /* UNSIGNED TINYINT*/

	SQL_C_BOOKMARK(-18),                       /* BOOKMARK        */

	SQL_C_GUID(-11)
	;
	


	
	@Getter
	private int type;
	
	private OdbcBindType(int type) {
		this.type = type;
	}
	public static OdbcBindType valueOf(int e) {
		for(OdbcBindType attr:OdbcBindType.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}
}
