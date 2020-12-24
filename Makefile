all: CPURAM
	g++ CPURAM.cpp -o CPURAM

install: CPURAM
	cp $< ../bin/

send2naga: CPURAM
	scp CPURAM sandy@167.172.5.72:/home/sandy/utils/bin

sendsrc2naga: CPURAM.cpp
	scp CPURAM.cpp sandy@167.172.5.72:/home/sandy/utils/src
	scp Makefile sandy@167.172.5.72:/home/sandy/utils/src

sendsrc2garuda: CPURAM.cpp
	scp CPURAM.cpp sandy@188.166.205.55:/home/sandy/utils/src
	scp Makefile sandy@188.166.205.55:/home/sandy/utils/src
