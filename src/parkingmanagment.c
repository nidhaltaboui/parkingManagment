#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parkingmanagment.h"


// Fonction pour chercher un parking par son ID dans le fichier
Parking chercher(const char *filename, char *id) {
    Parking p;
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return p; // Retourne un parking vide si le fichier n'est pas trouvé
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Extraire les données de chaque ligne
        sscanf(line, "%49[^;];%99[^;];%149[^;];%49[^;];%d;%99[^;];%49[^;];%d;%d",
               p.parking_id, p.parking_name, p.address, p.parking_type, &p.ability, p.opening_hours, p.pricing, &p.Confirm, &p.Cancel);

        // Si l'ID correspond, retourner ce parking
        if (strcmp(p.parking_id, id) == 0) {
            fclose(file);
            return p;
        }
    }

    fclose(file);
    // Retourner un parking vide si aucun parking n'a été trouvé
    memset(&p, 0, sizeof(Parking));
    return p;
}


// Fonction pour ajouter un parking dans le fichier
void ajouter_parking(const char *filename, Parking p) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    fprintf(file, "%s;%s;%s;%s;%d;%s;%s;%d;%d\n", 
            p.parking_id, p.parking_name, p.address, p.parking_type, 
            p.ability, p.opening_hours, p.pricing, p.Confirm, p.Cancel);
    fclose(file);
}

// Fonction pour modifier un parking dans le fichier
int modifier_parking(const char *filename, const char *id, Parking parking_modifie) {
    FILE *file = fopen(filename, "r+");  // Ouvre le fichier en lecture et écriture
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return 0;  // Retourne 0 si le fichier ne peut pas être ouvert
    }

    char line[1024];
    long pos;
    int found = 0;

    while ((pos = ftell(file)) >= 0 && fgets(line, sizeof(line), file)) {
        Parking parking_lu;
        sscanf(line, "%49[^;];%99[^;];%149[^;];%49[^;];%d;%99[^;];%49[^;];%d;%d",
               parking_lu.parking_id, parking_lu.parking_name, parking_lu.address,
               parking_lu.parking_type, &parking_lu.ability, parking_lu.opening_hours,
               parking_lu.pricing, &parking_lu.Confirm, &parking_lu.Cancel);

        // Si l'ID correspond, met à jour la ligne
        if (strcmp(parking_lu.parking_id, id) == 0) {
            fseek(file, pos, SEEK_SET);  // Place le curseur à la position où le parking a été trouvé
            fprintf(file, "%s;%s;%s;%s;%d;%s;%s;%d;%d\n",
                    parking_modifie.parking_id,
                    parking_modifie.parking_name,
                    parking_modifie.address,
                    parking_modifie.parking_type,
                    parking_modifie.ability,
                    parking_modifie.opening_hours,
                    parking_modifie.pricing,
                    parking_modifie.Confirm,
                    parking_modifie.Cancel);
            found = 1;
            break;
        }
    }

    fclose(file);
    return found;  // Retourne 1 si la modification a réussi, sinon 0
}


// Fonction pour supprimer un parking du fichier
void supprimer_parking(const char *filename, char *id) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        fclose(f);
        fprintf(stderr, "Erreur : Impossible de créer le fichier temporaire.\n");
        return;
    }

    Parking parking;
    char line[512];
    int found = 0;

    // Lire le fichier ligne par ligne
    while (fgets(line, sizeof(line), f)) {
        // Utilisation de sscanf pour lire une ligne et remplir la structure Parking
        if (sscanf(line, "%49[^;];%99[^;];%149[^;];%49[^;];%d;%99[^;];%49[^;];%d;%d",
                   parking.parking_id, parking.parking_name, parking.address,
                   parking.parking_type, &parking.ability, parking.opening_hours,
                   parking.pricing, &parking.Confirm, &parking.Cancel) == 9) {

            // Vérifier si l'ID correspond
            if (strcmp(parking.parking_id, id) == 0) {
                found = 1; // Marquer comme trouvé
                continue;  // Ne pas écrire cette ligne dans le fichier temporaire
            }
        }

        // Écrire les lignes non supprimées dans le fichier temporaire
        fprintf(temp, "%s", line);
    }

    fclose(f);
    fclose(temp);

    if (found) {
        // Supprimer l'ancien fichier et renommer le fichier temporaire
        if (remove(filename) != 0 || rename("temp.txt", filename) != 0) {
            fprintf(stderr, "Erreur : Impossible de mettre à jour le fichier %s.\n", filename);
        }
    } else {
        // Supprimer le fichier temporaire si l'ID n'a pas été trouvé
        remove("temp.txt");
        fprintf(stderr, "Erreur : Parking avec l'ID %s introuvable.\n", id);
    }
}
// Fonction pour afficher tous les parkings (utilisé pour débogage ou affichage dans l'interface)
void afficher_parkings(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

int affecter_agent(const char *filename, const char *id_parking, Agent a) {
    FILE *file_agents = fopen(filename, "r+");
    if (!file_agents) {
        g_print("Erreur : Impossible d'ouvrir le fichier des agents.\n");
        return 0; // Impossible d'ouvrir le fichier
    }

    char buffer[1024];
    int found = 0;
    long pos;

    while (fgets(buffer, sizeof(buffer), file_agents)) {
        pos = ftell(file_agents);  // Sauvegarde de la position actuelle

        // On compare l'ID du parking dans le fichier avec celui de l'agent
        if (strstr(buffer, id_parking) != NULL) {
            found = 1;
            fseek(file_agents, pos, SEEK_SET);  // Revenir à la position du début de l'agent
            fprintf(file_agents, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
                    a.id_parking, a.first_name, a.last_name, a.civil_status, a.cin, 
                    a.phone_number, a.email, a.address, a.job_title, a.contract_duration);
            fflush(file_agents);  // Sauvegarder immédiatement
            break;
        }
    }

    fclose(file_agents);

    return found ? 1 : 0;  // Si l'agent a été trouvé et mis à jour, retourner 1, sinon 0
}

Agent chercher_agent(const char *filename, const char *id_parking) {
    FILE *file_agents = fopen(filename, "r");
    Agent a = {0};  // Initialiser un agent avec des valeurs par défaut
    
    if (!file_agents) {
        // Impossible d'ouvrir le fichier
        printf("Erreur : Impossible d'ouvrir le fichier des agents.\n");
        return a;  // Retourner un agent vide
    }

    // Lire chaque ligne du fichier pour trouver l'agent
    while (fscanf(file_agents, "%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;];%99[^;]\n", 
                  a.id_parking, a.first_name, a.last_name, a.civil_status, a.cin, 
                  a.phone_number, a.email, a.address, a.job_title, a.contract_duration) == 10) {
        // Si l'ID de l'agent correspond à l'ID recherché
        if (strcmp(a.id_parking, id_parking) == 0) {
            fclose(file_agents);
            return a;  // Retourner l'agent trouvé
        }
    }

    fclose(file_agents);

    // Si l'agent n'a pas été trouvé, retourner un agent vide
    printf("Erreur : Agent introuvable pour l'ID %s.\n", id_parking);
    return a;
}

