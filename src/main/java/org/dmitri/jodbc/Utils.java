package org.dmitri.jodbc;

public class Utils {

	public static String toHex(Long data) {
		return "0x"+Integer.toHexString(data.intValue());
	}
}
