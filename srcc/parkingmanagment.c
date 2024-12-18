#include <stdio.h>
#include <string.h>
#include <ctype.h>

int validerParking(Parking p) {
    // Vérifie si l'identifiant du parking est non vide et ne dépasse pas 20 caractères
    if (strlen(p.parking_id) == 0 || strlen(p.parking_id) >= 20) {
        printf("Erreur: Identifiant de parking invalide\n");
        return 0;
    }

    // Vérifie si le nom du parking est non vide et ne dépasse pas 50 caractères
    if (strlen(p.parking_name) == 0 || strlen(p.parking_name) >= 50) {
        printf("Erreur: Nom de parking invalide\n");
        return 0;
    }

    // Vérifie si l'adresse est non vide et ne dépasse pas 100 caractères
    if (strlen(p.address) == 0 || strlen(p.address) >= 100) {
        printf("Erreur: Adresse de parking invalide\n");
        return 0;
    }

    // Vérifie que le format des heures d'ouverture est correct (par exemple "08:00-20:00")
    if (strlen(p.opening_hours) != 11 || p.opening_hours[2] != ':' || p.opening_hours[5] != '-') {
        printf("Erreur: Format des heures d'ouverture invalide\n");
        return 0;
    }

    // Vérifie que le tarif est un nombre suivi de "DT"
    if (strlen(p.pricing) < 3 || strlen(p.pricing) > 12) {
        printf("Erreur: Format du tarif invalide\n");
        return 0;
    }

    // Vérifie que tous les caractères avant "DT" sont des chiffres
    for (int i = 0; i < strlen(p.pricing) - 2; i++) {
        if (!isdigit(p.pricing[i])) {
            printf("Erreur: Le tarif doit être un nombre valide suivi de 'DT'\n");
            return 0;
        }
    }

    // Vérifie que les deux derniers caractères sont "DT"
    if (p.pricing[strlen(p.pricing) - 2] != 'D' || p.pricing[strlen(p.pricing) - 1] != 'T') {
        printf("Erreur: Le tarif doit se terminer par 'DT'\n");
        return 0;
    }

    return 1; // Tous les champs sont valides
}

// Fonction pour ajouter un parking
int ajouter(char *filename, Parking p) {
    FILE *f = fopen(filename, "r");
    Parking temp;

    // Vérifie si le parking existe déjà
    if (f != NULL) {
        while (fscanf(f, "%s %s %s %s %s %s\n",  // Ignorer ability et uniquement lire les champs nécessaires
                      temp.parking_id, temp.parking_name, temp.address, 
                      temp.opening_hours, temp.pricing) == 5) {
            if (strcmp(temp.parking_id, p.parking_id) == 0) {
                fclose(f);
                return -1; // Le parking existe déjà
            }
        }
        fclose(f);
    }

    // Ajoute le parking
    f = fopen(filename, "a");
    if (f != NULL) {
        fprintf(f, "%s;%s;%s;%s;%s\n",  // Format sans ability
                p.parking_id, p.parking_name, p.address, p.opening_hours, p.pricing);
        fclose(f);
        return 0; // Succès
    }
    return -1; // Erreur d'ouverture
}


// Fonction pour modifier un parking
int modifier(char *filename, char *parking_id, Parking new_data) {
    int found = 0;
    Parking p;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("temp.txt", "w");

    if (f != NULL && f2 != NULL) {
        while (fscanf(f, "%s %s %s %s %d %s %s %d %d %s\n", 
                      p.parking_id, p.parking_name, p.address, p.parking_type, 
                      &p.ability, p.opening_hours, p.pricing, &p.Confirm, 
                      &p.Cancel, p.agent_id) != EOF) {
            if (strcmp(p.parking_id, parking_id) == 0) {
                fprintf(f2, "%s %s %s %s %d %s %s %d %d %s\n", 
                        new_data.parking_id, new_data.parking_name, new_data.address, 
                        new_data.parking_type, new_data.ability, new_data.opening_hours, 
                        new_data.pricing, new_data.Confirm, new_data.Cancel, new_data.agent_id);
                found = 1;
            } else {
                fprintf(f2, "%s %s %s %s %d %s %s %d %d %s\n", 
                        p.parking_id, p.parking_name, p.address, p.parking_type, 
                        p.ability, p.opening_hours, p.pricing, p.Confirm, 
                        p.Cancel, p.agent_id);
            }
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("temp.txt", filename);
    }
    return found;
}

// Fonction pour supprimer un parking
int supprimer(char *filename, char *parking_id) {
    int found = 0;
    Parking p;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("temp.txt", "w");

    if (f != NULL && f2 != NULL) {
        while (fscanf(f, "%s %s %s %s %d %s %s %d %d %s\n", 
                      p.parking_id, p.parking_name, p.address, p.parking_type, 
                      &p.ability, p.opening_hours, p.pricing, &p.Confirm, 
                      &p.Cancel, p.agent_id) != EOF) {
            if (strcmp(p.parking_id, parking_id) != 0) {
                fprintf(f2, "%s %s %s %s %d %s %s %d %d %s\n", 
                        p.parking_id, p.parking_name, p.address, p.parking_type, 
                        p.ability, p.opening_hours, p.pricing, p.Confirm, 
                        p.Cancel, p.agent_id);
            } else {
                found = 1;
            }
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("temp.txt", filename);
    }
    return found;
}

// Fonction pour chercher un parking
Parking chercher(char *filename, char *parking_id) {
    Parking p = {"", "", "", "", 0, "", "", 0, 0};
    FILE *f = fopen(filename, "r");

    if (f != NULL) {
        while (fscanf(f, "%s %s %s %s %d %s %s %d %d %s\n", 
                      p.parking_id, p.parking_name, p.address, p.parking_type, 
                      &p.ability, p.opening_hours, p.pricing, &p.Confirm, 
                      &p.Cancel, p.agent_id) != EOF) {
            if (strcmp(p.parking_id, parking_id) == 0) {
                fclose(f);
                return p; // Parking trouvé
            }
        }
        fclose(f);
    }
    return p; // Retourne un parking vide si non trouvé
}

// Fonction pour affecter un agent à un parking
/*int affecterAgent(char *filename, char *parking_id, char *agent_id) {
    Parking p = chercher(filename, parking_id);
    if (strcmp(p.parking_id, "") != 0) {
        strcpy(p.agent_id, agent_id);
        return modifier(filename, parking_id, p);
    }
    return 0; // Parking non trouvé
}*/

