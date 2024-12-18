#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "parkingmanagment.h"





void on_Add_parking_clicked(GtkButton *button, gpointer user_data) {
    // Déclaration des widgets GTK
    GtkWidget *entry_id, *entry_name, *entry_address, *entry_hours, *entry_pricing;
    GtkWidget *spin_ability, *combobox_type, *radio_confirm, *radio_cancel;
    GtkWidget *label_status;

    // Déclaration de la structure Parking
    Parking p;

    // Nom du fichier
    char filename[] = "parkings.txt";

    // Récupération des widgets via lookup_widget
    entry_id = lookup_widget(GTK_WIDGET(button), "textentrynidhal1");
    entry_name = lookup_widget(GTK_WIDGET(button), "textentrynidhal2");
    entry_address = lookup_widget(GTK_WIDGET(button), "textentrynidhal3");
    entry_hours = lookup_widget(GTK_WIDGET(button), "textentrynidhal6");
    entry_pricing = lookup_widget(GTK_WIDGET(button), "textentrynidhal7");
    spin_ability = lookup_widget(GTK_WIDGET(button), "spinnidhal5");
    combobox_type = lookup_widget(GTK_WIDGET(button), "comboboxentry1");
    radio_confirm = lookup_widget(GTK_WIDGET(button), "radiobutton7");
    radio_cancel = lookup_widget(GTK_WIDGET(button), "radiobutton8");
    label_status = lookup_widget(GTK_WIDGET(button), "label118");

    // Récupération des textes entrés par l'utilisateur
    strcpy(p.parking_id, gtk_entry_get_text(GTK_ENTRY(entry_id)));
    strcpy(p.parking_name, gtk_entry_get_text(GTK_ENTRY(entry_name)));
    strcpy(p.address, gtk_entry_get_text(GTK_ENTRY(entry_address)));
    strcpy(p.opening_hours, gtk_entry_get_text(GTK_ENTRY(entry_hours)));
    strcpy(p.pricing, gtk_entry_get_text(GTK_ENTRY(entry_pricing)));

    // Validation des champs
    if (strlen(p.parking_id) == 0 || strlen(p.parking_name) == 0 || strlen(p.address) == 0 ||
        strlen(p.opening_hours) == 0 || strlen(p.pricing) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: All fields are required!");
        return;
    }

    // Vérification de la capacité
    p.ability = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_ability));
    if (p.ability <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Capacity must be greater than 0!");
        return;
    }

    // Vérification du type de parking
    int active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox_type));
    if (active_index == -1) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Parking type not selected!");
        return;
    } else {
        GtkTreeIter iter;
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox_type));
        gchar *parking_type = NULL;

        if (gtk_tree_model_get_iter_from_string(model, &iter, g_strdup_printf("%d", active_index))) {
            gtk_tree_model_get(model, &iter, 0, &parking_type, -1);
        }

        if (parking_type != NULL) {
            strcpy(p.parking_type, parking_type);
            g_free(parking_type);
        }
    }

    // Vérification des boutons radio
    gboolean confirm = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_confirm));
    gboolean cancel = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_cancel));

    if (!confirm && !cancel) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Please confirm or cancel!");
        return;
    }

    if (cancel) {
        gtk_label_set_text(GTK_LABEL(label_status), "Operation canceled. No parking added.");
        return;
    }

    // Si confirmé, ajouter le parking
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Unable to open file.");
        return;
    }

    fprintf(file, "%s;%s;%s;%s;%d;%s;%s\n",
            p.parking_id, p.parking_name, p.address, p.parking_type,
            p.ability, p.opening_hours, p.pricing);

    fclose(file);

    // Message de succès
    gtk_label_set_text(GTK_LABEL(label_status), "Parking added successfully!");

    // Réinitialiser les champs
    gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(entry_address), "");
    gtk_entry_set_text(GTK_ENTRY(entry_hours), "");
    gtk_entry_set_text(GTK_ENTRY(entry_pricing), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_ability), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_type), -1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_confirm), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_cancel), FALSE);
}






void
on_Search_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *check_id, *check_name, *check_address, *check_type;
    GtkWidget *entry_id, *entry_name, *entry_address, *combo_type;
    GtkWidget *treeview;
    GtkWidget *label_status;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GtkListStore *store;
    gboolean parking_found = FALSE;

    // Récupération des widgets
    check_id = lookup_widget(GTK_WIDGET(button), "checkbutton7");
    check_name = lookup_widget(GTK_WIDGET(button), "checkbutton8");
    check_address = lookup_widget(GTK_WIDGET(button), "checkbutton9");
    check_type = lookup_widget(GTK_WIDGET(button), "checkbutton10");  // Nouveau
    entry_id = lookup_widget(GTK_WIDGET(button), "entry27");
    entry_name = lookup_widget(GTK_WIDGET(button), "entry28");
    entry_address = lookup_widget(GTK_WIDGET(button), "entry25");
    combo_type = lookup_widget(GTK_WIDGET(button), "combo5");  // Nouveau
    treeview = lookup_widget(GTK_WIDGET(button), "treeview1");
    label_status = lookup_widget(GTK_WIDGET(button), "label120");

    // Récupération des textes/champs activés
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *name_text = gtk_entry_get_text(GTK_ENTRY(entry_name));
    const char *address_text = gtk_entry_get_text(GTK_ENTRY(entry_address));
    const char *type_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_type));  // Nouveau

    gboolean filter_by_id = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_id));
    gboolean filter_by_name = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_name));
    gboolean filter_by_address = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_address));
    gboolean filter_by_type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_type));  // Nouveau

    // Charger le fichier des parkings
    FILE *file = fopen("parkings.txt", "r");
    if (!file) {
        g_print("Error: Unable to open parking file.\n");
        gtk_label_set_text(GTK_LABEL(label_status), "File error");
        return;
    }

    // Créer ou réinitialiser le modèle pour le TreeView
    store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
                               G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, 
                               G_TYPE_STRING, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

    // Lire le fichier ligne par ligne
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        Parking p;
        sscanf(line, "%[^;];%[^;];%[^;];%[^;];%d;%[^;];%[^;];%d;%d",
               p.parking_id, p.parking_name, p.address, p.parking_type,
               &p.ability, p.opening_hours, p.pricing, &p.Confirm, &p.Cancel);

        // Appliquer les filtres
        gboolean matches = TRUE;

        if (filter_by_id && *id_text != '\0' && strstr(p.parking_id, id_text) == NULL) {
            matches = FALSE;
        }
        if (filter_by_name && *name_text != '\0' && strstr(p.parking_name, name_text) == NULL) {
            matches = FALSE;
        }
        if (filter_by_address && *address_text != '\0' && strstr(p.address, address_text) == NULL) {
            matches = FALSE;
        }
        if (filter_by_type && type_text != NULL) { // Nouveau
            if (strstr(p.parking_type, type_text) == NULL) { 
                matches = FALSE;
            }
        }

        // Ajouter au modèle si tous les filtres correspondent
        if (matches) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               0, p.parking_id,
                               1, p.parking_name,
                               2, p.address,
                               3, p.parking_type,
                               4, p.ability,
                               5, p.opening_hours,
                               6, p.pricing,
                               7, p.Confirm,
                               -1);
            parking_found = TRUE;
        }
    }

    fclose(file);

    // Libérer la mémoire pour la valeur de type_text
    if (type_text != NULL) g_free((gchar *)type_text);

    // Mettre à jour le label en fonction du résultat
    if (parking_found) {
        gtk_label_set_text(GTK_LABEL(label_status), "Parkings found");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "No parking found");
    }

    // Configurer les colonnes du TreeView si elles ne sont pas encore définies
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview)) == NULL) {
        renderer = gtk_cell_renderer_text_new();

        column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Adresse", renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 3, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Capacité", renderer, "text", 4, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Horaires", renderer, "text", 5, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Tarifs", renderer, "text", 6, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Confirmé", renderer, "text", 7, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    }
}






void
on_Assign_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry_id_agent = lookup_widget(GTK_WIDGET(button), "entry31");  // ID de l'agent
    GtkWidget *entry_id_parking = lookup_widget(GTK_WIDGET(button), "entry30");  // ID du parking
    GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label149");

    if (!entry_id_agent || !entry_id_parking || !label_status) {
        g_print("Error: Widgets not found.\n");
        return;
    }

    // Récupération des textes des ID (agent et parking)
    const char *id_agent_text = gtk_entry_get_text(GTK_ENTRY(entry_id_agent));
    const char *id_parking_text = gtk_entry_get_text(GTK_ENTRY(entry_id_parking));

    if (!id_agent_text || strlen(id_agent_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Please enter an Agent ID.");
        g_print("Error: Agent ID is empty.\n");
        return;
    }

    if (!id_parking_text || strlen(id_parking_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Please enter a parking ID.");
        g_print("Error: The parking ID is empty.\n");
        return;
    }

    // Recherche du parking par ID
    Parking p = chercher("parkings.txt", id_parking_text);
    if (strlen(p.parking_id) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Parking not found");
        g_print("Erreur : Parking introuvable pour l'ID %s.\n", id_parking_text);
        return;
    }

    // Recherche de l'agent par ID
    Agent a = chercher_agent("agents.txt", id_agent_text);
    if (strlen(a.id_parking) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Agent non trouvé");
        g_print("Error: Parking not found for ID %s.\n", id_agent_text);
        return;
    }

    // Vérification si l'agent est déjà affecté
    FILE *file = fopen("affectations.txt", "r");
    if (!file) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error opening assignment file.");
        g_print("Error opening file assignments.txt\n");
        return;
    }

    char line[256];
    char id_agent_in_file[50], id_parking_in_file[50];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "ID Parking: %[^,], ID Agent: %s", id_parking_in_file, id_agent_in_file);
        if (strcmp(id_agent_in_file, id_agent_text) == 0) {
            gtk_label_set_text(GTK_LABEL(label_status), "Error: Agent already assigned.");
            g_print("Erreur : L'agent %s est déjà affecté.\n", id_agent_text);
            fclose(file);
            return;
        }
    }
    fclose(file);

    // Ouvrir le fichier en mode ajout pour enregistrer la nouvelle affectation
    file = fopen("affectations.txt", "a");
    if (!file) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error opening assignment file.");
        g_print("Error opening file assignments.txt\n");
        return;
    }

    fprintf(file, "ID Parking: %s, ID Agent: %s\n", p.parking_id, id_agent_text);
    fclose(file);

    // Mise à jour du statut et impression du message
    gtk_label_set_text(GTK_LABEL(label_status), "Agent assigned to the parking.");
    g_print("Agent assigned to parking %s and added in assignments.txt.\n", p.parking_id);
}












void on_button22_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entry_id, *entry_parking_name, *entry_adress, *combo_parking_type, *spin_ability;
    GtkWidget *entry_opening_hours, *entry_pricing, *radiobutton_confirm, *radiobutton_cancel, *label_status;

    // Récupérer les widgets via lookup_widget
    entry_id = lookup_widget(GTK_WIDGET(button), "entry17");
    entry_parking_name = lookup_widget(GTK_WIDGET(button), "entry18");
    entry_adress = lookup_widget(GTK_WIDGET(button), "entry19");
    combo_parking_type = lookup_widget(GTK_WIDGET(button), "combo1");
    spin_ability = lookup_widget(GTK_WIDGET(button), "spinbutton2");
    entry_opening_hours = lookup_widget(GTK_WIDGET(button), "entry21");
    entry_pricing = lookup_widget(GTK_WIDGET(button), "entry22");
    radiobutton_confirm = lookup_widget(GTK_WIDGET(button), "radiobutton5");
    radiobutton_cancel = lookup_widget(GTK_WIDGET(button), "radiobutton6");
    label_status = lookup_widget(GTK_WIDGET(button), "label185");

    // Récupérer l'ID entré par l'utilisateur
    const char *search_id = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (!search_id || strlen(search_id) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Please enter a valid username.");
        return;
    }

    // Recherche dans le fichier de parkings
    Parking p = chercher("parkings.txt", (char *)search_id);

    // Si un parking est trouvé, remplir les champs
    if (strcmp(p.parking_id, "") != 0) {  // Parking trouvé
        gtk_entry_set_text(GTK_ENTRY(entry_parking_name), p.parking_name);
        gtk_entry_set_text(GTK_ENTRY(entry_adress), p.address);
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_parking_type), 
            strcmp(p.parking_type, "Outside") == 0 ? 0 : 1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_ability), p.ability);
        gtk_entry_set_text(GTK_ENTRY(entry_opening_hours), p.opening_hours);
        gtk_entry_set_text(GTK_ENTRY(entry_pricing), p.pricing);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_confirm), p.Confirm);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_cancel), p.Cancel);
        gtk_label_set_text(GTK_LABEL(label_status), "Parking successfully found!");
    } else {  // Parking non trouvé
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Parking not found.");
        // Réinitialisation des champs
        gtk_entry_set_text(GTK_ENTRY(entry_parking_name), "");
        gtk_entry_set_text(GTK_ENTRY(entry_adress), "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_parking_type), -1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_ability), 0);
        gtk_entry_set_text(GTK_ENTRY(entry_opening_hours), "");
        gtk_entry_set_text(GTK_ENTRY(entry_pricing), "");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_confirm), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_cancel), FALSE);
    }
}


void on_button21_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entry_id, *entry_parking_name, *entry_address;
    GtkWidget *combo_parking_type, *spin_ability;
    GtkWidget *entry_opening_hours, *entry_pricing;
    GtkWidget *radiobutton_confirm, *radiobutton_cancel;
    GtkWidget *label_status;

    Parking parking_modifie;
    char filename[] = "parkings.txt";

    // Récupération des widgets
    entry_id = lookup_widget(GTK_WIDGET(button), "entry17");
    entry_parking_name = lookup_widget(GTK_WIDGET(button), "entry18");
    entry_address = lookup_widget(GTK_WIDGET(button), "entry19");
    combo_parking_type = lookup_widget(GTK_WIDGET(button), "combo1");
    spin_ability = lookup_widget(GTK_WIDGET(button), "spinbutton2");
    entry_opening_hours = lookup_widget(GTK_WIDGET(button), "entry21");
    entry_pricing = lookup_widget(GTK_WIDGET(button), "entry22");
    radiobutton_confirm = lookup_widget(GTK_WIDGET(button), "radiobutton5");
    radiobutton_cancel = lookup_widget(GTK_WIDGET(button), "radiobutton6");
    label_status = lookup_widget(GTK_WIDGET(button), "label185");

    // Vérifier si le bouton cancel est activé
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_cancel))) {
        gtk_label_set_text(GTK_LABEL(label_status), "Modification annulée.");
        return; // Ne rien faire d'autre
    }

    // Vérifier si le bouton confirm est activé
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_confirm))) {
        gtk_label_set_text(GTK_LABEL(label_status), "Veuillez confirmer la modification.");
        return; // Empêcher la suite si aucun bouton n'est confirmé
    }

    // Récupérer les données modifiées
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *name_text = gtk_entry_get_text(GTK_ENTRY(entry_parking_name));
    const char *address_text = gtk_entry_get_text(GTK_ENTRY(entry_address));
    const char *parking_type_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_parking_type));
    const char *opening_hours_text = gtk_entry_get_text(GTK_ENTRY(entry_opening_hours));
    const char *pricing_text = gtk_entry_get_text(GTK_ENTRY(entry_pricing));

    // Vérification des champs obligatoires
    if (strlen(id_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: ID field is required.");
        return;
    }
    if (strlen(name_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Name field is required.");
        return;
    }
    if (strlen(address_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Address field is required.");
        return;
    }
    if (!parking_type_text || strlen(parking_type_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Parking type must be selected.");
        return;
    }
    if (strlen(opening_hours_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Opening hours field is required.");
        return;
    }
    if (strlen(pricing_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Pricing field is required.");
        return;
    }

    // Vérification des données numériques (prix et capacité)
    double pricing_value = atof(pricing_text);
    if (pricing_value <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Pricing must be a positive number.");
        return;
    }

    int ability_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_ability));
    if (ability_value <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Capacity must be greater than 0.");
        return;
    }

    // Remplir les champs de la structure
    strcpy(parking_modifie.parking_id, id_text);
    strcpy(parking_modifie.parking_name, name_text);
    strcpy(parking_modifie.address, address_text);
    strcpy(parking_modifie.parking_type, parking_type_text);
    strcpy(parking_modifie.opening_hours, opening_hours_text);
    strcpy(parking_modifie.pricing, pricing_text);
    parking_modifie.ability = ability_value;

    // Recherche du parking dans le fichier
    Parking parking_trouve = chercher(filename, id_text);
    if (strlen(parking_trouve.parking_id) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Parking not found.");
        return;
    }

    // Modifier le parking si trouvé
    if (modifier_parking(filename, parking_trouve.parking_id, parking_modifie)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Parking successfully modified!");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Error: Edit failed.");
    }
}





void
on_button23_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *treeview;
    GtkWidget *label_status; // Label pour afficher le statut
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    char parking_id[50]; // Pour stocker l'ID du parking sélectionné

    // Récupérer les widgets nécessaires
    treeview = lookup_widget(GTK_WIDGET(button), "treeview1");
    label_status = lookup_widget(GTK_WIDGET(button), "label120");

    // Récupération de l'ID dans l'Entry (entry27)
    GtkWidget *entry = lookup_widget(GTK_WIDGET(button), "entry27");
    const char *id = gtk_entry_get_text(GTK_ENTRY(entry));

    // Copier l'ID dans un tableau modifiable
    char id_modifiable[60];
    strcpy(id_modifiable, id);

    // Récupérer la sélection dans le TreeView
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        // Récupérer l'ID du parking sélectionné
        gtk_tree_model_get(model, &iter, 0, parking_id, -1);

        // Lire le fichier existant
        FILE *file = fopen("parkings.txt", "r");
        FILE *temp_file = fopen("temp.txt", "w");
        if (!file || !temp_file) {
            g_print("Error: Unable to open files.\n");
            gtk_label_set_text(GTK_LABEL(label_status), "File error");
            if (file) fclose(file);
            if (temp_file) fclose(temp_file);
            return;
        }

        // Copier les lignes sauf celle avec l'ID sélectionné
        char line[512];
        while (fgets(line, sizeof(line), file)) {
            Parking p;
            sscanf(line, "%[^;];%[^;];%[^;];%[^;];%d;%[^;];%[^;];%d;%d",
                   p.parking_id, p.parking_name, p.address, p.parking_type,
                   &p.ability, p.opening_hours, p.pricing, &p.Confirm, &p.Cancel);

            if (strcmp(p.parking_id, parking_id) != 0) {
                fputs(line, temp_file); // Conserver les lignes autres que celle à supprimer
            }
        }

        fclose(file);
        fclose(temp_file);

        // Remplacer l'ancien fichier par le nouveau
        remove("parkings.txt");
        rename("temp.txt", "parkings.txt");

        // Supprimer la ligne du modèle
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

        // Mettre à jour le label pour informer de la suppression
        gtk_label_set_text(GTK_LABEL(label_status), "Parking successfully deleted");
    } else {
        // Aucun élément sélectionné
        gtk_label_set_text(GTK_LABEL(label_status), "No parking selected");
    }

    // Supprimer le parking du fichier avec la fonction
    supprimer_parking("parkings.txt", id_modifiable);

    // Mettre à jour le label après suppression
    gtk_label_set_text(GTK_LABEL(label_status), "Parking supprimé avec succès.");
}


void
on_button24_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry_id, *treeview, *label_status;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GtkListStore *store;
    gboolean parking_found = FALSE;

    // Récupération des widgets
    entry_id = lookup_widget(GTK_WIDGET(button), "entry30");  // Champ de texte pour l'ID
    treeview = lookup_widget(GTK_WIDGET(button), "treeview5");  // Treeview pour afficher les résultats
    label_status = lookup_widget(GTK_WIDGET(button), "label149");  // Label pour afficher l'état de la recherche

    // Récupération du texte de l'ID
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));

    // Charger le fichier des parkings
    FILE *file = fopen("parkings.txt", "r");
    if (!file) {
        g_print("Error: Unable to open parking file.\n");
        gtk_label_set_text(GTK_LABEL(label_status), "File error");
        return;
    }

    // Créer ou réinitialiser le modèle pour le TreeView
    store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
                               G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, 
                               G_TYPE_STRING, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

    // Lire le fichier ligne par ligne
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        Parking p;
        sscanf(line, "%[^;];%[^;];%[^;];%[^;];%d;%[^;];%[^;];%d;%d",
               p.parking_id, p.parking_name, p.address, p.parking_type,
               &p.ability, p.opening_hours, p.pricing, &p.Confirm, &p.Cancel);

        // Appliquer le filtre sur l'ID uniquement
        if (*id_text != '\0' && strstr(p.parking_id, id_text) != NULL) {
            // Si l'ID correspond, ajouter au modèle
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               0, p.parking_id,
                               1, p.parking_name,
                               2, p.address,
                               3, p.parking_type,
                               4, p.ability,
                               5, p.opening_hours,
                               6, p.pricing,
                               7, p.Confirm,
                               -1);
            parking_found = TRUE;
        }
    }

    fclose(file);

    // Mettre à jour le label en fonction du résultat
    if (parking_found) {
        gtk_label_set_text(GTK_LABEL(label_status), "Parking found");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "No parking found");
    }

    // Configurer les colonnes du TreeView si elles ne sont pas encore définies
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview)) == NULL) {
        renderer = gtk_cell_renderer_text_new();

        column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Adresse", renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 3, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Capacité", renderer, "text", 4, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Horaires", renderer, "text", 5, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Tarifs", renderer, "text", 6, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        column = gtk_tree_view_column_new_with_attributes("Confirmé", renderer, "text", 7, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    }
}




