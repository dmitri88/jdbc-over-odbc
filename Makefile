JODBC_INCLUDES = -I/usr/lib/jvm/java-11-openjdk-i386/include/linux -I/opt/jdbc-src/jodbc -I/usr/include/i386-linux-gnu -I/usr/lib/jvm/java-11-openjdk-i386/include
JODBC_FLAGS = -std=c++11 -Wall -Wno-unused-variable -Wno-write-strings  -fcompare-debug-second -DUNICODE
LIBS_INCLUDE = -L/usr/lib/jvm/java-11-openjdk-i386/lib/server  -L/opt/jdbc-src/bin -L/usr/lib/i386-linux-gnu

all:
	make clean
	make library
	make java
	make test-app
	
clean:
	rm -f .libs/libjodbc.so jodbc/*.o test/*.o test/*.a	bin/*.o bin/*.a
	
library:
	docker exec build-jdbc-odbc sh -c "cd /opt/jdbc-src && make docker_lib"
test-app:
	docker exec build-jdbc-odbc sh -c "cd /opt/jdbc-src && make docker_test"

java:
	gradle jar	

docker_lib: compiled.obj
	gcc -m32 -shared $(LIBS_INCLUDE) -o bin/libjodbc.so bin/jvm.o bin/jodbc.o bin/unicode.o bin/jdatabase.o bin/jstatement.o bin/jdescriptor.o -ljvm -lodbcinst

compiled.obj: unicode.cpp jodbc.cpp jvm.cpp jdatabase.cpp jstatement.cpp jdescriptor.cpp

jodbc.cpp:
	gcc -m32 -c $(JODBC_FLAGS)  $(JODBC_INCLUDES) jodbc/jodbc.cpp -o bin/jodbc.o
jvm.cpp:
	gcc -m32 -c $(JODBC_FLAGS)  $(JODBC_INCLUDES) jodbc/jvm.cpp   -o bin/jvm.o
jdatabase.cpp:
	gcc -m32 -c $(JODBC_FLAGS)  $(JODBC_INCLUDES) jodbc/jdatabase.cpp   -o bin/jdatabase.o
jstatement.cpp:
	gcc -m32 -c $(JODBC_FLAGS)  $(JODBC_INCLUDES) jodbc/jstatement.cpp   -o bin/jstatement.o
jdescriptor.cpp:
	gcc -m32 -c $(JODBC_FLAGS)  $(JODBC_INCLUDES) jodbc/jdescriptor.cpp   -o bin/jdescriptor.o
unicode.cpp:
	gcc -m32 -c $(JODBC_FLAGS)  $(JODBC_INCLUDES) jodbc/unicode.cpp -o bin/unicode.o
	
docker_test.obj:
	gcc -c -m32 $(JODBC_FLAGS) $(LIBS_INCLUDE) $(JODBC_INCLUDES) test/test-jdbc.cpp -L/usr/lib/i386-linux-gnu -L/usr/lib/jvm/java-11-openjdk-i386/lib/server \
  	-I/usr/include/x86_64-linux-gnu  \
 	-o bin/test-jdbc.a   
docker_test: docker_test.obj
	gcc -m32 $(LIBS_INCLUDE) -o bin/test-jdbc.o bin/test-jdbc.a -ljodbc -ljvm	 -lstdc++ -lpthread
	 	