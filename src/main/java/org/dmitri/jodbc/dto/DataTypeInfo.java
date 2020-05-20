package org.dmitri.jodbc.dto;

import lombok.Data;

@Data
public class DataTypeInfo {

	private String name;
	private int typeId;
	private boolean unsigned;
	private int size;
}
