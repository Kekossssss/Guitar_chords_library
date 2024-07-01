# TP7 : Fichier Makefile
#
include ../Makefile.inc

EXE = serveur client

all: ${EXE}

${EXE): ${PSE_LIB}

serveur: serveur.o fonctionsTraitementUtilisateurs.o

serveur: serveur.o accords.o

clean:
	rm -f *.o *~ ${EXE}


