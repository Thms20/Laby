#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include <vector>
#include <cmath>

using namespace std;

class Labyrinthe;

class Gardien : public Mover {
private:
    bool bo = true;
	int nb_update = 0;
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{}

	// mon gardien pense tr�s mal!
	//void update (void) {};
	void update () {
		if(nb_update == 150) {
			changeAngle();
			nb_update = 0;
		}
		else nb_update++;


		if(bo) { // juste pour tester une fois le find, à enlever pour après
			find();
			bo = false;
		}



	}

	// et ne bouge pas!
	bool move (double dx, double dy) { return false; }


	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}


	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }



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

		float pas = 0.05; // Le pas de déplacement en abscisse

		float dx = x;
		float dy = y;


		float newX = abs(sx);
		float newY = abs(sy);

		char objet;
		float facteur;


		if (newY >= newY) { // On regarde si la longueur entre le chasseur et le gardien est plus grand en x ou y sur le nouveau repère tout simplement
			facteur = (ychasseur > y) ? 1.0f : -1.0f;
			while ((facteur > 0 && dy < ychasseur) || (facteur < 0 && dy > ychasseur)) {
				dy += facteur * pas;
				dx = a * dy + b;
				objet = _l->data(int(dx), int(dy)); // pour voir ce qu'il y a dans la grille à une case donnée
			//	std::cout << int(dx) << " " << int(dy) << std::endl;
			}
		}
		else {
			facteur = (xchasseur > x) ? 1.0f : -1.0f;
			while ((facteur > 0 && dx < xchasseur) || (facteur < 0 && dx > xchasseur)) {
				dx += facteur * pas; // On se rapproche en abscisse du chasseur
				dy = my * dx + iy; // pareil en ordonné mais tout en étant sur notre droite (du gardien vers le chasseur)
				objet = _l->data(int(dx), int(dy));
			//	std::cout << int(dx) << " " << int(dy) << std::endl;
        	}
   		}




		return true;
	}

};

#endif
