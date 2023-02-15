#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

/**
@Combeau Thomas & Kabongo Ben
Classe header du chasseur, ici on définie les entêtes de fonctions et le constructeur du chasseur.
*/

class Labyrinthe;

class Chasseur : public Mover {
private:
	// Vie de départ du chasseur.
	int hp = 50;
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
	// Du point de vue de chasseur on peut savoir si la partie est finie, cela permet de stopper toutes les interactions possibles du chasseur.
	bool finPartie = false;
public:
	// les sons.
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;		// on a tap� un mur.

	// constructeur.
	Chasseur (Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		gagner();
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	void update (void) {};
	// fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi.
	void fire (int angle_vertical);
	// clic droit.
	void right_click (bool shift, bool control);
	// Test si le joueur a gagné
	void gagner();
	// Chasseur perd de la vie
	void perdVie();
};

#endif
