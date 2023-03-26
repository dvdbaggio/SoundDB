#include <iostream>
#include <cstdio>
#include <fstream>
#include "dependencies/include/libpq-fe.h"


using namespace std;

PGconn* connect(const char* host, const char* user, const char* db, const char* pass, const char* port) {
    char conninfo[256];
    sprintf(conninfo, "user=%s password=%s dbname=\'%s\' hostaddr=%s port=%s",
        user, pass, db, host, port);

    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Cannot connect to the database" << endl << PQerrorMessage(conn);
        PQfinish(conn);
        exit(1);
    }

    return conn;
}

int menu() {
	cout << "----------------------------------------------------QUERIES----------------------------------------------------" << endl;
	cout << "	1 - Contare quante canzoni appartenenti ad uno stesso artista compaiono almeno due volte nelle playlist" << endl;
	cout << "	2 - Contare quanti album ha pubblicato un artista e mostrare il suo nome" << endl;
	cout << "	3 - Mostrare il nome dell'album e dell'artista con il numero di canzoni in ordine decrescente" << endl;
    cout << "	4 - Mostrare il nome del podcaster e del suo podcast con il numero di episodi in ordine decrescente" << endl;
    cout << "	5 - Mostrare il nickname dell'utente e contare il numero di pagamenti effettuati" << endl;
    cout << "	6 - ESCI" << endl;
	cout << "---------------------------------------------------------------------------------------------------------------" << endl;

	int x;
	cout << "Scegli: " << endl;
	cin >> x;

	return x;
}

void tableLines(int campi, int* maxLength) {
    for(int i = 0; i < campi; i++) {
        cout << '+';
        for(int j = 0; j < maxLength[i] + 2; j++)
            cout << '-';
    }
    cout << '+' << endl;
}

void printQuery(PGresult* res) {
    int tuples = PQntuples(res);
    int fields = PQnfields(res);

    string queryCols[tuples + 1][fields];

    for(int i = 0; i < fields; i++)
        queryCols[0][i] = PQfname(res, i);

    for(int i = 0; i < tuples; i++) {
        for(int j = 0; j < fields; j++)
            queryCols[i+1][j] = PQgetvalue(res, i, j);
    }

    int maxLength[fields];
    for(int i = 0; i < fields; i++) {
        maxLength[i] = 0;
        for(int j = 0; j < tuples+1; j++) {
            int size = queryCols[j][i].size();
            maxLength[i] = size > maxLength[i] ? size : maxLength[i];
        }
    }

    tableLines(fields, maxLength);
    for(int i = 0; i < fields; i++) {
        cout << "| ";
        cout << queryCols[0][i];
        for(int j = 0; j < maxLength[i] - queryCols[0][i].size() + 1; j++)
            cout << ' ';
        if(i == fields-1)
            cout << '|';
    }
    cout << endl;
    tableLines(fields, maxLength);
    for(int i = 1; i < tuples+1; i++) {
        for(int j = 0; j < fields; j++) {
            cout << "| ";
            cout << queryCols[i][j];
            for(int k = 0; k < maxLength[j] - queryCols[i][j].size() + 1; k++)
                cout << ' ';
            if(j == fields - 1)
                cout << '|';
        }
        cout << endl;
    }
    tableLines(fields, maxLength);
}

void checkResults(PGresult *res, const PGconn *conn) {
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cout << "Error: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        exit(1);
    }
}

int main(int argc, char **argv) {

    cout << "Database name: ";
    char database_name[50];
    cin >> database_name;

    cout << "Username: ";
    char username[50];
    cin >> username;

    cout << "Password: ";
    char pass[50];
    cin >> pass;
    PGconn* conn = connect("127.0.0.1", username, database_name, pass, "5432");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    else {
        cout << "Connection to database succeeded." << endl;
    }

    const char* queries [5] = {
        "SELECT artista.nome_artista, count(canzone.titolo)  \
         FROM Contenuto_playlist, canzone, album, artista    \
         WHERE Contenuto_playlist.album = canzone.album      \
         AND contenuto_playlist.titolo = canzone.titolo      \
         AND canzone.album = album.album_id                  \
         AND album.artista = artista.mail                    \
         GROUP BY artista.nome_artista                       \
         HAVING count(canzone.titolo) > 1;",

        "SELECT artista.nome_artista, count(album.titolo) AS num_album  \
         FROM album, artista                                            \
         WHERE album.artista = artista.mail                             \
         GROUP BY artista.nome_artista                                  \
         ORDER BY num_album ASC;",  

        "SELECT album.titolo, artista.nome_artista, count(canzone.titolo) AS num_canzoni    \
         FROM album, artista, canzone                                                       \
         WHERE album.artista = artista.mail                                                 \
         AND album.album_id = canzone.album                                                 \
         GROUP BY album.titolo, artista.nome_artista                                        \
         ORDER BY num_canzoni DESC;",

        "SELECT podcaster.nome, podcast.nome_podcast, count(episodio.titolo) AS num_episodi   \
         FROM podcaster, podcast, episodio                                              \
         WHERE podcaster.mail = podcast.podcaster                                       \
         AND podcast.podcast_id = episodio.podcast                                      \
         GROUP BY podcaster.nome, podcast.nome_podcast                                        \
         ORDER BY num_episodi DESC;",
 
        "SELECT utente.nickname, count(pagamento.transazione_id) AS num_pagamenti   \
         FROM utente, pagamento, dati_fatturazione                                  \
         WHERE utente.mail = dati_fatturazione.Utente                               \
         AND dati_fatturazione.fatturazione_id = pagamento.fatturazione_id          \
         GROUP BY utente.nickname                                                   \
         ORDER BY num_pagamenti DESC;"
    };

    PGresult *res;
    int choice;

    do {
		choice = menu();
		switch (choice) {
		case 1:
			system("clear");
            res = PQexec(conn, queries[choice - 1]);
            checkResults(res, conn);
            printQuery(res);
			break;
		case 2:
			system("clear");
            res = PQexec(conn, queries[choice - 1]);
            checkResults(res, conn);
            printQuery(res);
			break;
        case 3:
            system("clear");
            res = PQexec(conn, queries[choice - 1]);
            checkResults(res, conn);
            printQuery(res);
			break;
        case 4:
            system("clear");
            res = PQexec(conn, queries[choice - 1]);
            checkResults(res, conn);
            printQuery(res);
			break;
        case 5:
            system("clear");
            res = PQexec(conn, queries[choice - 1]);
            checkResults(res, conn);
            printQuery(res);
            break;
		case 6:
			system("clear");
			cout << "Stai uscendo dal programma...\n\n";
			break;
		default:
			system("clear");
			cout << "Inserimento errato!" << endl;	
		}
	} while (choice != 6);

    PQclear(res);
    PQfinish(conn);

    return 0;
}