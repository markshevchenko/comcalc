all: comcalc.exe

comcalc.exe:
	cl comcalc.cpp parser.cpp scanner.cpp /Fecomcalc.exe

clean:
	del /S /Q *.obj