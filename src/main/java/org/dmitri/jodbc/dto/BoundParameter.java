package org.dmitri.jodbc.dto;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class BoundParameter {
	private int column;	
	private int type;	
	private Long buffer;
	private Long bufLen;
	private Long retBuffer;
}
