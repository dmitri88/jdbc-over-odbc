package org.dmitri.jodbc.dto;

import org.dmitri.jodbc.enums.OdbcBindType;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class BoundParameter {
	private int column;	
	private OdbcBindType type;	
	private Long buffer;
	private Long bufLen;
	private Long retBuffer;
}
