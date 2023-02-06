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
	{_angle = 270;}

	// mon gardien pense tr�s mal!
	//void update (void) {};
	void update () {
	


	//	if(bo) { // juste pour tester une fois le find, à enlever pour après
		//	find();
		//	bo = false;
	//	}

	if(nb_update == 1) {
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
				cout << _angle << endl;
			}
			nb_update = 0;
		}
		else nb_update++;



	}

	// et ne bouge pas!
	bool move (double dx, double dy) {
		 if(_l->data(dx / Environnement::scale, dy / Environnement::scale) == 1) {
			return false;
		}
		return true;
	}


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
			while ((facteur > 0 && dy < ychasseur) || (facteur < 0 && dy > ychasseur)) { // Probleme quand l'abscisse ou l'ordonnée sont égales
				dy += facteur * pas;
				dx = a * dy + b;
				objet = _l->data(int(dx), int(dy)); // pour voir ce qu'il y a dans la grille à une case donnée
				cout << (int)objet << " " << dx << " " << dy << endl;

				if((int) objet == 1) {
					bo = false;
					return 0;
				}
			}
		}
		else {
			facteur = (xchasseur > x) ? 1.0f : -1.0f;
			while ((facteur > 0 && dx < xchasseur) || (facteur < 0 && dx > xchasseur)) {
				dx += facteur * pas; // On se rapproche en abscisse du chasseur
				dy = my * dx + iy; // pareil en ordonné mais tout en étant sur notre droite (du gardien vers le chasseur)
				objet = _l->data(int(dx), int(dy));
				cout << (int)objet << " " << dx << " " << dy << endl;

				if((int) objet == 1) {
					bo = false;
					return 0;
				}
        	}
   		}

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

};

#endif
