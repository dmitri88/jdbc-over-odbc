package org.dmitri.jodbc;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@RequiredArgsConstructor
public class OdbcStatementWrapper {
	
	private final long statementId;

	public void freeResource(long option) {
		log.debug("JAVA freeResource {} {}",statementId,option);
	}

	public void execDirect(String sql) {
		log.debug("JAVA execDirect {} {}",statementId,sql);
		
	}

}
