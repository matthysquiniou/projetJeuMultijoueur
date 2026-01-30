## Installation et génération de la solution

1. Aller dans le dossier `bin/`.
2. Exécuter `make.bat`.  
3. Le script génère un dossier `ide/` contenant `[SOLUTION_NAME].sln`.
4. Ouvrir la solution Visual Studio.

# Projet Réseau – Jeu Multijoueur UDP

## Description

Ce projet est un projet réseau réalisé dans le cadre scolaire.  
Il a été développé afin d’apprendre et de pratiquer :

- La programmation réseau
- L’utilisation des **threads**
- La communication via le protocole **UDP**

## Fonctionnement général

Le projet est composé de deux parties :

- **Un serveur**
- **Un ou plusieurs clients**

### Serveur

Le serveur doit être lancé en premier.  
Il permet aux clients de se connecter et de communiquer entre eux.

### Client

Une fois le client lancé, il demande à l’utilisateur de saisir :

1. Une **adresse IPv4**
2. Un **port**
3. Un **nom** pour jouer

Une fois ces informations renseignées, le joueur peut rejoindre la partie.

## Contrôles

- **Z Q S D** : se déplacer
- **Clic gauche** : tirer
- **Clic droit** : suivre le dernier projectile tiré

## Remarques

Ce projet a été réalisé à des fins pédagogiques et sert principalement à illustrer les concepts de réseau et de multithreading.
