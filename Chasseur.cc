#include "Chasseur.h"
#include "Gardien.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include <string>
#include <cmath>

/**
@Combeau Thomas & Kabongo Ben
Classe chasseur qui implémente les fonctions définies dans chasseur.h.
*/

/*
 *	Tente un deplacement et test aussi si le chasseur passe par une case où il y a une mark au sol. Si c'est
 *	le cas alors le chasseur se téléporte à une autre mark aléatoirement parmi celles dans le labyrinthe.
 */
bool Chasseur::move_aux (double dx, double dy)
{
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
        
        // si position == position d'une marque -> teleportation sur une marque aleatoire
        bool teleportation = false;
        int j = 0;
        for (int i = 0; i < _l -> _nmarks; i++) {
            if (_l -> _marks [i]._x == (int)(_x/Environnement::scale) && _l -> _marks [i]._y == (int)(_y/Environnement::scale)) {
                teleportation = true;
                j = i;
                break;
            }
        }
        
        if (teleportation) {
            // choix aleatoire de la position
            int i = rand() % (_l -> _nmarks);
            if (j == i) {if (j == 0) i = 1; else i = j - 1;}
            _x = _l -> _marks [i]._x*Environnement::scale;
            _y = _l -> _marks [i]._y*Environnement::scale;
        }

		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu et test si la boule de feu touche un gardien en regardant la distance entre la boule de feu et chaque gardien,
 *	si c'est le cas, le gardien perd de la vie. 
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

// ------------------
// Test si un gardien est touché par une boule de feu
	float dist;
	for(int i = 1; i < _l->_nguards; i++) {
		Gardien* guard = (Gardien*) (_l->_guards[i]);
		if(guard->hp > 0) { // Ce test permet que si le gardien meurt, il n'arrete plus les boules de feu.
			dist = sqrt((guard->_x - _fb -> get_x ())/Environnement::scale * (guard->_x - _fb -> get_x ())/Environnement::scale + 
						(guard->_y - _fb -> get_y ())/Environnement::scale * (guard->_y - _fb -> get_y ())/Environnement::scale);

			if(dist < 1) {
				guard->etreToucheParBdf();
				return false;
			}
		}
	}
// ---------------------



	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visee */ angle_vertical, _angle);
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Chasseur::right_click (bool shift, bool control)
{
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
}


/*
 * Regarde si le joueur gagne selon sa distance avec le trésor.
*/
void Chasseur::gagner() {
	if( sqrt((Mover::_l->_treasor._x - (_x - float(Mover::_l->scale)/2)/float(Mover::_l->scale))*(Mover::_l->_treasor._x - (_x - float(Mover::_l->scale)/2)/float(Mover::_l->scale)) + 
	(Mover::_l->_treasor._y  - (_y - float(Mover::_l->scale)/2)/float(Mover::_l->scale)) * (Mover::_l->_treasor._y - (_y - float(Mover::_l->scale)/2)/float(Mover::_l->scale))) 
	<= 2) {
		if(!finPartie) {
			finPartie = true;
			partie_terminee(true);
		}
	}
}

/*
* Chasseur qui perd de la vie
*/
void Chasseur::perdVie() {
	this->hp -= 5;

	if(hp <= 0 && finPartie == false) {
		finPartie = true;
		partie_terminee(false);
	}
}
