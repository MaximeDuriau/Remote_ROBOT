#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <curl/curl.h>

// Fonction pour envoyer les données au serveur
void sendDataToServer(const char *data)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://votre-serveur.com/endpoint");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

int main()
{
    // Initialisation de ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int ch;
    while ((ch = getch()) != 'q')
    { // Sortie de la boucle si la touche 'q' est pressée
        // Si une touche de direction est pressée, envoyer l'information au serveur
        switch (ch)
        {
        case KEY_UP:
            sendDataToServer("UP");
            break;
        case KEY_DOWN:
            sendDataToServer("DOWN");
            break;
        case KEY_LEFT:
            sendDataToServer("LEFT");
            break;
        case KEY_RIGHT:
            sendDataToServer("RIGHT");
            break;
        }
    }

    // Fin de ncurses
    endwin();

    return 0;
}
