//
//  accords.h
//  
//
//  Created by Némo Kardassevitch on 29/05/2024.
//

#ifndef accords_h
#define accords_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


/*Construction nom fichier: nbCordes_tailleAccord_premFrette_tuning_accord_variantes.txt*/

/*Structure instruments, ce sont les paramètres du profil utilisateurs*/
typedef struct {
    char *tuning; /*Contient le tuning (accordage) des cordes*/
    int nbCordes; /*Contient le nombre de cordes de l'instrument utilisateur*/
    int nbFrettes; /*Contient le nombre de frettes de l'instrument de l'utilisateur*/
} Instruments;

/*Structure accords*/
typedef struct {
    char *nomAccord; /*Contient le nom de l'accord*/
    int premFrette; /*Contient le numéro de la première frette utile de l'accord (0 pour la corde à vide)*/
    int tailleAccord; /*Contient le nombre de frettes utiles pour l'accord*/
} Accords;

/*Structure pour les accès fichiers*/
typedef struct {
    char * dossierProfil;
    char * dossierAccord;
} Acces;

/*Structure représentation*/
typedef struct {
    int **T; /*Matrice représentant l'accord*/
    Instruments *instrument;
    Accords *accord;
    Acces *acces;
} Representation;






//Initialisations


/*Initialisation des accords*/
void instrument_Init (Instruments *I, char *tuning, int nbC, int nbF);

/*Initialisation des instruments*/
void accord_Init (Accords *A, char *nom, int pF, int taille);

/*Initialisation des accès*/
void acces_Init (Acces *Ac, char *dossierProfil, char *dossierAccord);

/*Initialisation de la représentation*/
void representation_Init (Representation *R, Instruments *I, Accords *A, Acces *Ac);

/*Permet l'initialisation de toutes les structures à partir des données du profil transmise et de la recherche d'accord effectuée*/
void recherche_Init (Representation *R, char *tuning, int nbC, int nbF, char *nom, int pF, int taille);



//Free


/*Libère le malloc du tuning*/
void free_Instrument (Instruments *I);

/*Libère le malloc du nom*/
void free_Accord (Accords *A);

/*Libère les mallocs des accès*/
void free_Acces (Acces *Ac);

/*Libère les malloc de la matrice et des deux structures à laquelle il est lié*/
void free_Representation (Representation *R);

/*Un seul free pour les libérer tous*/
void free_Tout (Representation *R);



//Affichages


/*Affiche les données importantes de l'instrument*/
void affiche_Instrument (Instruments *I, int canal);

/*Affiche les données importantes de l'accord*/
void affiche_Accord (Accords *A, int canal);

/*Affiche la représentation de l'accord*/
void affiche_Representation (Representation *R, int canal);

/*Affichage de toutes les structures*/
void affiche_Tout (Representation *R, int canal);

/*Affichage des variantes existantes*/
void affiche_Variantes (Representation *R, int **LV, int canal);



//Organisation des dossiers


/*Création du dossier profil*/
void dossier_Profil (Representation *R);

/*Création du dossier accord*/
void dossier_Accord (Representation *R);

/*Vérifie qu'un fichier existe*/
int fichier_Existe (Representation *R);

/*Renvoie le nombre de fichier (donc de variantes) dans un dossier*/
int nb_Variantes (Representation *R);



//Lecture et écriture des fichiers

/*Écriture*/
void ecriture (Representation *R);

/*Lecture*/
void lecture (Representation *R);



//Choix utilisateurs


/*Choix d'une variante parmis celles proposées*/
void choix_Variante (Representation *R, int canal);

/*Choix de la taille et de la première frette lors de la création d'un nouvel accord*/
void choix_Construction (Representation *R, int canal);


//Actions


/*Création d'un nouvel accord*/
void nouvel_Accord (Representation *R, int canal);

/*Permet de consulter un accord*/
void consult_Accord (Representation *R, int canal);

/*Modification d'un accord*/
void modifie_Accord (Representation *R, int canal);










#endif /* accords_h */
