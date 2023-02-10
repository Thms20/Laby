#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

class Labyrinthe;

class Gardien : public Mover {
private:
	int nb_update = 0;
	int hp = 20; // Vie du gardien
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{_angle = 270;}



	// mon gardien pense tr�s mal!
	void update () {
		if(hp > 0) {
			if(find()) {
				attack();

				if(nb_update == 100) {
					fire(0);
					nb_update = 0;
				}
				else nb_update++;
			}
			else patrouille();
		}

}

	// et ne bouge pas!
	bool move (double dx, double dy) {
		 if(_l->data(dx / Environnement::scale, dy / Environnement::scale) == 1) {
			return false;
		}
		return true;
	}


	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {
		_fb->init(_x, _y, 10.0, angle_vertical, -_angle);
	}


	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { 
		int x = int((_fb->get_x() + dx) / float(Environnement::scale));
		int y = int((_fb->get_y() + dy) / float(Environnement::scale));


		// ------------------
		// Test si le chasseur est touché par une boule de feu
		float dist;
		Chasseur* guard = (Chasseur*) (_l->_guards[0]);
		dist = sqrt(((guard->_x - _fb -> get_x ())/Environnement::scale) * ((guard->_x - _fb -> get_x ())/Environnement::scale) + 
					((guard->_y - _fb -> get_y ())/Environnement::scale) * ((guard->_y - _fb -> get_y ())/Environnement::scale));

		if(dist < 1) {
			guard->perdVie();
			return false;
		}
		// ---------------------

		return ((int) _l->data(x, y) == 1) ? false : true;
	}



	// Change d'angle
	void changeAngle() {
		std::vector<int> direction = {0, 90, 180, 270};
		int r = rand()%4;

		_angle = direction[r];
	}

	// Cherche le chasseur
	bool find() {
		float xchasseur, ychasseur, x, y;

		xchasseur = _l->_guards[0]->_x/Environnement::scale;
		ychasseur = _l->_guards[0]->_y/Environnement::scale;

		x = _x/Environnement::scale;
		y = _y/Environnement::scale;


		float sx, sy;
		
		sx = xchasseur - x;
		sy = ychasseur - y;

		// y = ax + b
		float a, b;
		a = sx/sy;
		b = x - a * y; // x et y inversé

		float my, iy;
		my = sy / sx; // Pente dans le sens inverse d'une courbe !
    	iy = y - my * x; // ordonné sur la longueur dX entre guillemet

		float pas = 0.02; // Le pas de déplacement en abscisse

		float dx = x;
		float dy = y;


		float newX = abs(sx);
		float newY = abs(sy);

		char objet;
		float facteur;


		if(dx == xchasseur || dy == ychasseur) return false;

		if (newY >= newX) { // On regarde si la longueur entre le chasseur et le gardien est plus grand en x ou y sur le nouveau repère tout simplement
			facteur = (ychasseur > y) ? 1.0f : -1.0f;
			while ((facteur > 0 && dy < ychasseur) || (facteur < 0 && dy > ychasseur)) { // Probleme quand l'abscisse ou l'ordonnée sont égales
				dy += facteur * pas;
				dx = a * dy + b;
				objet = _l->data(int(dx), int(dy)); // pour voir ce qu'il y a dans la grille à une case donnée

				if((int) objet == 1) {
					return false;
				}
			}
		}
		else {
			facteur = (xchasseur > x) ? 1.0f : -1.0f;
			while ((facteur > 0 && dx < xchasseur) || (facteur < 0 && dx > xchasseur)) {
				dx += facteur * pas; // On se rapproche en abscisse du chasseur
				dy = my * dx + iy; // pareil en ordonné mais tout en étant sur notre droite (du gardien vers le chasseur)
				objet = _l->data(int(dx), int(dy));

				if((int) objet == 1) {
					//bo = false;
					return false;
				}
        	}
   		}

		float angle = atan(sx / sy);
		int rapport = (sy > 0) ? 0 : 180;
		int degree = int(angle * -180.0f / M_PI);
		_angle = degree + rapport;

		return true;
	}


	bool verifAvancement(int angle) {
		switch(angle) {
			case 0:
				return move(_x, _y + 5);
				break;
			case 90:
				return move(_x - 5, _y);
				break;
			case 180:
				return move(_x, _y - 5);
				break;
			case 270:
				return move(_x + 5, _y);
				break;
		}
		
	}


void patrouille() {
	if(_angle != 0 && _angle != 90 && _angle != 180 && _angle != 270 ) changeAngle();
	if(verifAvancement(_angle)) {
		switch(_angle) {
			case 0:
				_y += 10/Environnement::scale;
				break;
			case 90:
				_x -= 10/Environnement::scale;
				break;
			case 180:
				_y -= 10/Environnement::scale;
				break;
			case 270:
				_x += 10/Environnement::scale;
				break;
		}
	}
	else {
		changeAngle();
	}
}

void attack() {
		float xchasseur, ychasseur, x, y;

		xchasseur = _l->_guards[0]->_x/Environnement::scale;
		ychasseur = _l->_guards[0]->_y/Environnement::scale;

		x = _x/Environnement::scale;
		y = _y/Environnement::scale;


		float sx, sy;
		
		sx = xchasseur - x;
		sy = ychasseur - y;

		float a, b;
		a = sx/sy;
		b = x - a * y;

		float my, iy;
		my = sy / sx;
    	iy = y - my * x;

		float pas = 0.02;

		float dx = x;
		float dy = y;


		float newX = abs(sx);
		float newY = abs(sy);

		float facteur;


		if (newY >= newX) {
			facteur = (ychasseur > y) ? 1.0f : -1.0f;
			dy += facteur * pas;
			_y = dy * Environnement::scale;
			_x = (a * dy + b) * Environnement::scale;

		}
		else {
			facteur = (xchasseur > x) ? 1.0f : -1.0f;
			dx += facteur * pas;
			_x = dx * Environnement::scale;
			_y = (my * dx + iy) * Environnement::scale;
   		}
}



void etreToucheParBdf() {
	hp -= 5;
	if(hp <= 0) rester_au_sol();
}

};

#endif
