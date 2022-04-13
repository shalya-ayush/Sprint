CC = gcc
CFLAGS = -g -Wall -c
LFLAGS = -lrt -lpthread
INCLUDEPATH = ./../include
SRCPATH = ./../src
OBJPATH = ./../obj
BINPATH = ./../bin


$(BINPATH)/voz: $(OBJPATH)/voz.o $(OBJPATH)/text.o  $(OBJPATH)/sip.o  $(OBJPATH)/rtp.o 
	$(CC) -o voz $(OBJPATH)/voz.o  $(OBJPATH)/text.o  $(OBJPATH)/sip.o  $(OBJPATH)/rtp.o 
	mv voz $(BINPATH)
$(OBJPATH)/voz.o: $(SRCPATH)/voz.c 
	$(CC) $(CFLAGS) $(SRCPATH)/voz.c -I $(INCLUDEPATH)/ 
	mv voz.o $(OBJPATH)/
$(OBJPATH)/text.o: $(SRCPATH)/text.c $(INCLUDEPATH)/header.h  $(INCLUDEPATH)/text.h 
	$(CC) $(CFLAGS) $(SRCPATH)/text.c -I $(INCLUDEPATH)/ 
	mv text.o $(OBJPATH)/

$(OBJPATH)/sip.o: $(SRCPATH)/sip.c $(INCLUDEPATH)/header.h  $(INCLUDEPATH)/sip.h 
	$(CC) $(CFLAGS) $(SRCPATH)/sip.c -I $(INCLUDEPATH)/ 
	mv sip.o $(OBJPATH)/
	
$(OBJPATH)/rtp.o: $(SRCPATH)/rtp.c $(INCLUDEPATH)/header.h  $(INCLUDEPATH)/rtp.h $(INCLUDEPATH)/sip.h 
	$(CC) $(CFLAGS) $(SRCPATH)/rtp.c -I $(INCLUDEPATH)/ 
	mv rtp.o $(OBJPATH)/
clear:
	rm $(BINPATH)/*
	rm $(OBJPATH)/*
