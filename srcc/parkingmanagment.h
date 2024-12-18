#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED

#include <stdio.h>

// Structure représentant un parking
typedef struct {
    char parking_id[20];        // Identifiant unique du parking
    char parking_name[50];      // Nom du parking
    char address[100];          // Adresse du parking
    char parking_type[20];      // Type de parking (ex : ouvert, fermé)
    int ability;                // Capacité d'accueil du parking
    char opening_hours[20];     // Heures d'ouverture
    char pricing[10];           // Tarif
    int Confirm;                // Confirmation
    int Cancel;                 // Annulation
    char agent_id[20];          // Identifiant de l'agent
} Parking;

// Fonctions de gestion de parking
int ajouter(char *filename, Parking p);
int modifier(char *filename, char *parking_id, Parking new_data);
int supprimer(char *filename, char *parking_id);
Parking chercher(char *filename, char *parking_id);
int affecterAgent(char *filename, char *parking_id, char *agent_id);
int validerParking(Parking p); // Ajoutez cette ligne si elle manque

#endif // PARKING_H_INCLUDED

