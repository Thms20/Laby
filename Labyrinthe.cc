#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include <string>
#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"

using namespace std;

/*
* @Kabongo Ben & Combeau Thomas
*
* Cette classe permet de construire notre labyrinthe en utilisant les objets définis dans Environnement et en lisant un fichier texte qui decrit l'ensemble du
* labyrinthe comme par exemple où sont placés les objets comme les murs, les boites, le tresor, les personnages,... Cette classe a donc pour principale fonction
* de parser un fichier texte.
*/

Sound*    Chasseur::_hunter_fire;    // bruit de l'arme du chasseur.
Sound*    Chasseur::_hunter_hit;    // cri du chasseur touchÈ.
Sound*    Chasseur::_wall_hit;    // on a tapÈ un mur.


Environnement* Environnement::init (char* fluxname) {
    return new Labyrinthe(fluxname);
}

// structure pour les positions
struct Position {
    int x;
    int y;
};

// structure pour les murs
struct WallPosition {
    int x1;
    int x2;
    int y1;
    int y2;
    bool vertical; // true si le mur est vertical
};

// structure pour les affiches
struct PictPosition {
    Position position;  // position de l'affiche
    string name;        // nom de l'affiche
};


/*
*
* Le constructeur de la classe Labyrinthe est implémenté dans le fichier "Labyrinthe.cpp". Il prend en paramètre le nom d'un fichier de configuration de labyrinthe.
*
* Le constructeur effectue les opérations suivantes :
*
* Il initialise quelques variables locales, notamment la largeur maximale de toutes les lignes du fichier.
* Il ouvre le fichier spécifié en lecture. Si le fichier n'existe pas, il affiche un message d'erreur et quitte.
* Il lit le fichier ligne par ligne en ignorant les lignes vides ou commentées.
* Il récupère les noms des images à afficher à partir des premiers caractères en minuscules dans les lignes.
* Il calcule la hauteur du labyrinthe à partir du nombre de lignes dans le fichier.
* Pour chaque caractère dans chaque ligne, le constructeur identifie le type d'objet à cet emplacement :
* si le caractère est 'X', il ajoute cette position aux positions des boîtes;
* si le caractère est 'M', il ajoute cette position aux positions des marques;
* si le caractère est une lettre minuscule ('a' à 'z'), il ajoute cette position et le nom de l'image associée à la liste des positions et noms d'images;
* si le caractère est 'G', il ajoute cette position aux positions des gardiens;
* si le caractère est 'C', il enregistre la position du chasseur;
* si le caractère est 'T', il enregistre la position du trésor;
* si le caractère est '-', il vérifie si un mur horizontal est en cours de construction;
* si le caractère est '|', il vérifie si un mur vertical est en cours de construction;
* si le caractère est ' ', il termine la construction du mur horizontal en cours.
* 
* Le constructeur fournit donc le labyrinthe initialisé avec les positions et noms d'images, positions des gardiens, positions des boîtes, positions des marques,
* positions des trésors, la position du chasseur et les positions des murs.
*
* Pour la construction des murs, on récupère le point de départ de chaque mur et le point de fin (qu’on peut deviner grâce au character « + »),
* on fait la différence entre les murs verticaux et horizontaux grâce à l’attribut booléen « vertical » dans la structure représentant chaque mur : « WallPosition ».
*
* Les murs sont ensuite construit en utilisant les coordonnées des éléments de la liste wallPositions pour déterminer leur position dans le labyrinthe.
*
*/
Labyrinthe::Labyrinthe(char* fluxname) {
    int width  = 0;
    int height = 0;

    int plusGrandeLigneLargeur = 0;

    // Test si le fichier labyrinthe.txt existe
    ifstream flux(fluxname);
    if (!flux) {
        cout << "ERREUR: Impossible d'ouvrir le fichier de labyrinthe en lecture." << endl;
        // quitter le programme ??
    }
    
    else {
        // noms des affiches :: clé : code -  valeur : fichier de l'affiche
        unordered_map<char, string> affiches;
        
        // positions
        list<Position> guardsPositions(0);   // les gardiens (x, y)
        list<Position> boxesPositions(0);    // les boîtes
        list<Position> marksPositions(0);    // les marques
	Position chasseurPos;               // La position du chasseur
	chasseurPos.x = 0;
	chasseurPos.y = 0;
        list<Position> tresorsPositions(0);  // les trésors
        list<PictPosition> pictsPositions(0);    // les affiches
        
        list<WallPosition> wallPositions(0);     // les murs (x1, y1, x2, y2)
        
        // noms des affiches en fonction des positions
        list<string> pictsNames(0);
        
        // gestion des murs verticaux :: clé : colonne du mur - valeur : mur
        unordered_map<int, WallPosition> verticalWalls;
        
        string line;
        while (getline(flux, line)) {
            if (line.empty() || line[0] == '#') {
                continue; // ignore les lignes vides ou qui commencent par un commentaire
            }
            
            // récupère les noms des affiches
            if(line[0] >= 'a' && line[0] <= 'z') {
                size_t pos = line.find_first_of("\t ");
                pos = line.find_first_not_of("\t ", pos);
                size_t pos2 = line.find_first_of("\t ", pos);
                affiches[line[0]] = line.substr(pos, pos2-pos);
                continue;
            }

	    // Compare la taille de la ligne pour obtenir la plus grande
	    plusGrandeLigneLargeur = (plusGrandeLigneLargeur > (int) line.length()) ? plusGrandeLigneLargeur : (int) line.length();
            
            int j = height;
	    height++;
            
            bool horizontalWall(false);
            WallPosition horizontalWallPosition;
            horizontalWallPosition.vertical = false;
            
            // récupération de la position des objets
            for (int i = 0; i < (int)line.length(); i++) {
                char c = line[i];

                // boites
                if (c == 'X') boxesPositions.push_back({i, j});
                 // marks
                else if (c == 'M') marksPositions.push_back({i, j});
                // affiches
                else if (c >= 'a' && c <= 'z') {
                    Position position = {i, j};
                    string name("");
                    if (affiches.count(c) > 0) name = affiches[c];
                    pictsPositions.push_back({position, name});
                }
                // gardien
                else if (c == 'G') guardsPositions.push_back({i, j});
                // chasseur
                else if (c == 'C')  {
					chasseurPos.x = i;
					chasseurPos.y = j;
				}
                // trésor
                else if (c == 'T') tresorsPositions.push_back({i, j});
                // continuité d'un mur horizontal
                else if (c == '-') {
                    if (!horizontalWall) {
                        // bizarre !
                    }
                }
                // continuité d'un mur vertical
                else if (c == '|') {
                    if (verticalWalls.count(i) == 0) {
                        // bizarre !
                    }
                }
                else if (c == ' ') {
                    horizontalWall = false; // fin du mur horizontal
                    // fin du mur vertical sur la colonne s'il existe
                    if (verticalWalls.count(i) > 0) verticalWalls.erase(i);
                }
                // début et fin de mur
                else if (c == '+') {
                    // horizontal
                    if (horizontalWall) {
                        // fin d'un mur horizontal sur la ligne s'il existe
                        horizontalWallPosition.x2 = i; // position du mur
                        wallPositions.push_back(horizontalWallPosition); // ajout du mur
                        // les murs horizontaux peuvent se chevaucher -> début d'un nouveau mur
                    }
                    // début d'un nouveau mur horizontal
                    horizontalWall = true;
                    horizontalWallPosition.x1 = i;
                    horizontalWallPosition.y1 = j;
                    horizontalWallPosition.y2 = j;
                    
                    // vertical
                    if (verticalWalls.count(i) > 0) {
                        // fin du mur vertical sur la colonne s'il existe
                        verticalWalls[i].y2 = j; // position du mur
                        wallPositions.push_back(verticalWalls[i]); // ajout du mur
                        verticalWalls.erase(i); // suppression du mur
                        // les murs verticaux peuvent se chevaucher -> début d'un nouveau mur
                    }
                    // début d'un nouveau mur vertical
                    WallPosition wall;
                    wall.x1 = i;
                    wall.x2 = i;
                    wall.y1 = j;
                    wall.vertical = true;
                    verticalWalls[i] = wall;
                }
            }
        }
        
        // nombre d'objets
        _nwall      = (int) wallPositions.size();
        _npicts     = (int) pictsPositions.size();
        _nboxes     = (int) boxesPositions.size();
        _nmarks     = (int) marksPositions.size();
        _nguards    = /*(int) huntersPositions.size() */ 1  + (int) guardsPositions.size();

        
        _walls  = new Wall[_nwall];
        _picts  = new Wall[_npicts];
        _boxes  = new Box[_nboxes];
        _marks  = new Box[_nmarks];
        _guards = new Mover*[_nguards];


        // set width and height
	setHeight(height * 2);
	width = plusGrandeLigneLargeur;
	setWidth(width * 2);
		
        _data = new char* [Labyrinthe::width()];
        for (int i = 0; i < Labyrinthe::width(); ++i)
            _data [i] = new char [Labyrinthe::height ()];


        // placements des objets
        
        // On place le trésor
	
        _treasor._x = tresorsPositions.front().x;
        _treasor._y = tresorsPositions.front().y;
        _data [_treasor._x][_treasor._y] = 1;
        
        int i;
        
        // On place les murs 
        i = 0;
        for (auto const& wall : wallPositions) {
            _walls[i]._x1 = wall.x1 ;
            _walls[i]._y1 = wall.y1 ;
            _walls[i]._x2 = wall.x2 ;
            _walls[i]._y2 = wall.y2 ;
            _walls[i]._ntex = 0;
            
            for (int k = wall.x1; k <= wall.x2; k++) {
                for (int l = wall.y1; l <= wall.y2; l++) {
                    if (k == wall.x1 || k == wall.x2 || l == wall.y1 || l == wall.y2)
                        _data[k][l] = 1;
                    else
                        _data[k][l] = EMPTY;
                }
            }
            i++;
        }
        
        // On place les affiches
        i = 0;
        for (auto const& pict : pictsPositions) {
            // les affiches doivent faire 2 de long)
            _picts[i]._x1 = pict.position.x;
            _picts[i]._x2 = pict.position.x + 2;
            _picts[i]._y1 = pict.position.y;
            _picts[i]._y2 = pict.position.y;
            _picts[i]._ntex = 0;
            
            if (!pict.name.empty()) {
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "%s/%s", texture_dir, pict.name.c_str());
                _picts[i]._ntex = wall_texture(tmp);
            }
            
            i++;
        }
        
        // On place les boxes
        i = 0;
        for (auto const& boxe : boxesPositions) {
            _boxes[i]._x = boxe.x;
            _boxes[i]._y = boxe.y;
            _boxes[i]._ntex = 0;
            _data[_boxes [i]._x][_boxes [i]._y] = 1;
            
            if (i % 2 == 0) {
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "%s/%s", texture_dir, "boite.jpg");
                _boxes[i]._ntex = wall_texture(tmp);
            }
            else if (i % 3 == 0) {
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "%s/%s", texture_dir, "boite-2.jpg");
                _boxes[i]._ntex = wall_texture(tmp);
            }
            
            i++;
        }

        // On place les marques
        i = 0;
        for (auto const& mark : marksPositions) {
            _marks[i]._x = mark.x;
            _marks[i]._y = mark.y;
            _marks[i]._ntex = 0;
            _data[_marks [i]._x][_marks [i]._y] = 0;
            
            if (i % 2 == 0) {
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "%s/%s", texture_dir, "p2.jpg");
                _marks[i]._ntex = wall_texture(tmp);
            }
            else if (i % 3 == 0) {
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "%s/%s", texture_dir, "p2.png");
                _marks[i]._ntex = wall_texture(tmp);
            }
            
            i++;
        }
        
        //  On cree le chasseur et le place dans le labyrinthe
        i = 0;
            _guards[i] = new Chasseur(this);
            _guards[0]->_x = float(chasseurPos.x) * float(scale) + float(scale) / 2.0f;
            _guards[0]->_y = float(chasseurPos.y) * float(scale) + float(scale) / 2.0f;
            i++;


       // On cree et on place les gardiens
       int index;

       char* styles[4] = {"Lezard", "Blade", "Serpent", "Samourai"};
        for (auto const& guard : guardsPositions) {
            index = rand()%4;
            _guards[i] = new Gardien(this, styles[index]);
            _guards[i]->_x = float(guard.x) * float(scale) + float(scale) / 2.0f;
            _guards[i]->_y = float(guard.y) * float(scale) + float(scale) / 2.0f;
            i++;
        }
        


        
    }
}
