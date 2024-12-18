#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "parkingmanagment.h"

void
on_Add_clicked                         (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Display_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Modify_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void on_Add_parking_clicked(GtkButton *button, gpointer user_data) {
    // Déclaration des widgets GTK
    GtkWidget *entry_id, *entry_name, *entry_address, *entry_hours, *entry_pricing;
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

    label_status = lookup_widget(GTK_WIDGET(button), "label118");

    // Récupération des textes entrés par l'utilisateur
    strcpy(p.parking_id, gtk_entry_get_text(GTK_ENTRY(entry_id)));
    strcpy(p.parking_name, gtk_entry_get_text(GTK_ENTRY(entry_name)));
    strcpy(p.address, gtk_entry_get_text(GTK_ENTRY(entry_address)));
    strcpy(p.opening_hours, gtk_entry_get_text(GTK_ENTRY(entry_hours)));
    strcpy(p.pricing, gtk_entry_get_text(GTK_ENTRY(entry_pricing)));

    // Validation des champs
    if (!validerParking(p)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Données invalides");
        return;  // Arrête l'exécution si la validation échoue
    }

    // Écriture dans le fichier texte
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        // Assurez-vous que le format est correct pour chaque champ
        fprintf(file, "%s;%s;%s;%s;%s\n",
                p.parking_id,
                p.parking_name,
                p.address,
                p.opening_hours,
                p.pricing);
        fclose(file);

        // Message de succès
        gtk_label_set_text(GTK_LABEL(label_status), "Parking ajouté avec succès !");
    } else {
        // Message d'erreur si le fichier ne peut pas être ouvert
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Impossible d'ouvrir le fichier.");
    }

    // Réinitialiser les champs du formulaire après l'ajout
    gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(entry_address), "");
    gtk_entry_set_text(GTK_ENTRY(entry_hours), "");
    gtk_entry_set_text(GTK_ENTRY(entry_pricing), "");
}


void
on_Search_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Search_parking_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Modify_parking_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Assign_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_Remove_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}

