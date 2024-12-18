#ifndef PARKINGMANAGEMENT_H
#define PARKINGMANAGEMENT_H

// Structure pour représenter un parking
typedef struct {
    char parking_id[50];
    char parking_name[100];
    char address[150];
    char parking_type[50];
    int ability;
    char opening_hours[100];
    char pricing[50];
    int Confirm; // 1 pour confirmé, 0 pour non confirmé
    int Cancel;  // 1 pour annulé, 0 pour non annulé
} Parking;
typedef struct {
    char id_parking[60];
    char first_name[60];
    char last_name[60];
    
    char civil_status[60];
    char cin[60];
    char phone_number[60];
    char email[60];
    char address[60];
    char job_title[60];
    char contract_duration[60];
}Agent;

// Fonction pour chercher un parking par son ID dans un fichier
Parking chercher(const char *filename, char *id) ;


// Fonction pour ajouter un parking dans le fichier
void ajouter_parking(const char *filename, Parking p);

// Fonction pour modifier un parking dans le fichier
int modifier_parking(const char *filename, const char *id, Parking parking_modifie) ;

// Fonction pour supprimer un parking du fichier
void supprimer_parking(const char *filename, char *id);

// Fonction pour afficher tous les parkings (utilisé pour débogage ou affichage dans l'interface)
void afficher_parkings(const char *filename);

// Déclaration de la fonction affecter_agent
int affecter_agent(const char *filename, const char *id_parking, Agent a);

// Fonction pour chercher un agent par son ID dans un fichier
Agent chercher_agent(const char *filename, const char *id_parking);



#endif

