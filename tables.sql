DROP TABLE IF EXISTS Utente CASCADE;
DROP TABLE IF EXISTS Artista CASCADE;
DROP TABLE IF EXISTS Podcaster CASCADE;
DROP TABLE IF EXISTS Album CASCADE;
DROP TABLE IF EXISTS Podcast CASCADE;
DROP TABLE IF EXISTS Canzone CASCADE;
DROP TABLE IF EXISTS Episodio CASCADE;
DROP TABLE IF EXISTS Abbonamento CASCADE;
DROP TABLE IF EXISTS Playlist CASCADE;
DROP TABLE IF EXISTS Contenuto_playlist CASCADE;
DROP TABLE IF EXISTS piano CASCADE;
DROP TABLE IF EXISTS categoria CASCADE;
DROP TABLE IF EXISTS Carta_di_credito CASCADE;
DROP TABLE IF EXISTS podcast_categoria CASCADE;
DROP TABLE IF EXISTS dati_fatturazione CASCADE;
DROP TABLE IF EXISTS Pagamento CASCADE;

CREATE TABLE Utente (
	mail VARCHAR (50) PRIMARY KEY,
	nome VARCHAR (50) NOT NULL,
	cognome VARCHAR (50) NOT NULL,
	stato CHAR (2),
	password VARCHAR (30) NOT NULL,
	nickname VARCHAR (20) NOT NULL UNIQUE,
	followers INT,
	following INT
);

CREATE TABLE Artista (
	mail VARCHAR (50) PRIMARY KEY,
	nome VARCHAR (50) NOT NULL,
	cognome VARCHAR (50) NOT NULL,
	stato CHAR (2),
	password VARCHAR (30) NOT NULL,
	followers INT,
	nome_artista VARCHAR (30) NOT NULL UNIQUE,
	info VARCHAR (200),
	label VARCHAR (20),
	ascolti_mensili INT
);

CREATE TABLE Podcaster (
	mail VARCHAR(50) PRIMARY KEY,
	nome VARCHAR(50) NOT NULL,
	cognome VARCHAR(50) NOT NULL,
	stato VARCHAR(50),
	password VARCHAR(50) NOT NULL UNIQUE,
	followers INT,
	nome_podcaster VARCHAR(30) NOT NULL UNIQUE,
	info VARCHAR(200)
);

CREATE TABLE Album (
	album_id SERIAL PRIMARY KEY,
	titolo VARCHAR (30) NOT NULL,
	artista VARCHAR (50) NOT NULL,
	data_pubblicazione DATE NOT NULL,
	FOREIGN KEY (artista) REFERENCES Artista(mail)
);
TRUNCATE ONLY Album
RESTART IDENTITY;

CREATE TABLE Podcast (
    podcast_id SERIAL PRIMARY KEY,
	nome_podcast VARCHAR(30) NOT NULL,
	podcaster VARCHAR(50) NOT NULL,
	info VARCHAR(100),
	FOREIGN KEY (podcaster) REFERENCES Podcaster(mail)
);
TRUNCATE ONLY Podcast
RESTART IDENTITY;

CREATE TABLE Canzone (
	titolo VARCHAR (30),
	album INT,
	durata INT NOT NULL,
	PRIMARY KEY (titolo, album),
    FOREIGN KEY (album) REFERENCES Album(album_id)
);

CREATE TABLE Episodio (
	podcast INT,
	titolo VARCHAR(50) NOT NULL,
	descrizione VARCHAR(300),
	durata INT NOT NULL,
	data_pubblicazione DATE NOT NULL,
	PRIMARY KEY (titolo, podcast),
	FOREIGN KEY (podcast) references Podcast(podcast_id)
);

CREATE TABLE Abbonamento (
    nome VARCHAR(20) PRIMARY KEY CHECK (nome IN('FREE', 'PREMIUM', 'FAMILY')),
    costo_mensile NUMERIC (4, 2) CHECK (costo_mensile >= 0),
    descrizione VARCHAR(200)
);

CREATE TABLE Playlist (
	playlist_id CHAR (10) PRIMARY KEY,
	nome VARCHAR (30) NOT NULL,
	descrizione VARCHAR (250),
	data_creazione DATE NOT NULL,
	utente VARCHAR (50) NOT NULL,
	FOREIGN KEY (utente) REFERENCES Utente(mail)
);

CREATE TABLE Contenuto_playlist (
    playlist CHAR (10) NOT NULL,
    titolo VARCHAR (30),
    album INT,
    FOREIGN KEY (playlist) REFERENCES Playlist(playlist_id),
    FOREIGN KEY (titolo, album) REFERENCES Canzone(titolo, album)
);

CREATE TABLE piano (
    utente VARCHAR(50) PRIMARY KEY,
    inizio_piano DATE NOT NULL,
    fine_piano DATE NOT NULL,
    CHECK (EXTRACT (MONTH FROM fine_piano) - EXTRACT (MONTH FROM inizio_piano) >= 1 OR EXTRACT (YEAR FROM fine_piano) - EXTRACT (YEAR FROM inizio_piano) >= 1),
    abbonamento VARCHAR (20),
    FOREIGN KEY (utente) references utente(mail),
    FOREIGN KEY (abbonamento) references abbonamento(nome)
);

CREATE TABLE categoria (
    nome VARCHAR (30) PRIMARY KEY,
    descrizione VARCHAR(100)
);

CREATE TABLE Carta_di_credito (
    numero NUMERIC(16,0) PRIMARY KEY,
    cvv NUMERIC(3,0) NOT NULL,
    intestatario VARCHAR(50) NOT NULL,
    scadenza DATE NOT NULL,
    circuito VARCHAR (20) NOT NULL
    CHECK (circuito IN('mastercard', 'visa', 'maestro'))
);

CREATE TABLE podcast_categoria (
    podcast INT NOT NULL,
    categoria VARCHAR(30) NOT NULL,
    FOREIGN KEY (podcast) REFERENCES podcast(podcast_id),
    FOREIGN KEY (categoria) REFERENCES categoria(nome)
);

CREATE TABLE dati_fatturazione (
    fatturazione_id CHAR(10) PRIMARY KEY,
    codice_fiscale CHAR(16) UNIQUE NOT NULL,
    civico INT NOT NULL,
    via VARCHAR(30) NOT NULL,
    citta VARCHAR(30) NOT NULL,
    cap INT CHECK (cap between 10000 and 99999) NOT NULL,
    stato CHAR(2) NOT NULL,
    utente VARCHAR (50) NOT NULL,
    carta_credito NUMERIC(16,0) NOT NULL,
    FOREIGN KEY (utente) REFERENCES utente(mail),
    FOREIGN KEY (carta_credito) REFERENCES carta_di_credito(numero)
);

CREATE TABLE Pagamento (
    transazione_id CHAR (10) PRIMARY KEY,
    fatturazione_id CHAR (10) NOT NULL,
    data_fattura DATE NOT NULL,
    importo NUMERIC (4, 2) NOT NULL,
    FOREIGN KEY (fatturazione_id) REFERENCES Dati_fatturazione(fatturazione_id),
    CHECK(importo >= 0)
);