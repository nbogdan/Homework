all:	build run clean


build:
	cup -nowarn -nosummary *.cup
	jflex -q *.jflex
	javac -cp ./java-cup-11a-runtime.jar *.java

run:
	java Checker Main.class  --args "-cp .:*" 

clean:
	rm sym*
	rm *Lexer.java
	mv Checker.class Checker.clas
	rm *.class
	mv Checker.clas Checker.class
