#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include <vector>
#include <cmath>
#include <iostream>

/**
*@Combeau Thomas & Kabongo Ben
*
* Classe header gardien, on a fait le choix d'implementer directement les fonctions ici donc cette classe contient les entetes de fonctions et
* les implementations de tout ce qui concerne chaque gardien.
*/

using namespace std;

class Labyrinthe;

class Gardien : public Mover {
private:
	int nb_update = 0; // Le nombre d'update va permettre de gerer l'apparition de la boule de feu des gardiens dans la fonction update.
public:
	int hp = 20; // Vie du gardien
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{_angle = 270;}



	/*
	*  Fonction update qui regarde premierement si le gardien est encore en vie (si il est mort il ne fait plus rien).
	* Si il est encore en vie, il regarde si il voit le chasseur avec la fonction find, si c'est le cas il rentre en mode attaque
	*  et tire une boule de feu toutes les 100 updates (100 appels de la fonction update). Si il ne voit pas le chasseur, il rentre
	*  en mode patrouille.
	*/
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

	/*
	* Move est une fonction qui teste si une case est accessible ou non en regardant si cette case contient une boite ou mur.
	*/
	bool move (double dx, double dy) {
		 if(_l->data(dx / Environnement::scale, dy / Environnement::scale) == 1) {
			return false;
		}
		return true;
	}


	/*
	* Fire initialise le tire de la boule de feu du gardien selon la position du gardien et les angles vertical et de direction.
	*/
	void fire (int angle_vertical) {
		_fb->init(_x, _y, 10.0, angle_vertical, -_angle);
	}


	/*
	* 	Fait bouger la boule de feu et test si la boule de feu touche le chasseur en regardant la distance entre la boule de feu et le chasseur,
 	*	si c'est le cas, le chasseur perd de la vie. 
	*/
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



	/*
	* Permet au gardien de changer aleatoirement de direction entre 4 directions.
	*/
	void changeAngle() {
		std::vector<int> direction = {0, 90, 180, 270};
		int r = rand()%4;

		_angle = direction[r];
	}

	/*
	* La fonction find permet pour un gardien de chercher le chasseur. Le principe est de se placer dans un nouveau repere selon deux points :
	*  les coordonnees du chasseur et du gardien. On test toutes les cases sur la droite entre le gardien et le chasseur pour voir si il y a un obstacle,
	*  on utilise un pas très petit de 0,02 pour ne pas louper un coin d'un obstacle par exemple. Enfin, si le gardien voit le chasseur alors on change
	*  la direction du gardien vers le chasseur.
	*/
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

		// Probleme quand le gardien et le chasseur sont sur une meme position en abscisse ou ordonne, on a donc choisi
		// de renvoyer false directement pour ne plus avoir ce probleme, en jeu cela ne modifie pas l'experience.
		if(dx == xchasseur || dy == ychasseur) return false;

		if (newY >= newX) { // On regarde si la longueur entre le chasseur et le gardien est plus grand en x ou y sur le nouveau repère
			facteur = (ychasseur > y) ? 1.0f : -1.0f;
			while ((facteur > 0 && dy < ychasseur) || (facteur < 0 && dy > ychasseur)) {
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
				dy = my * dx + iy; // on calcule la position en ordonné selon le nouveau dx pour rester sur notre droite (du gardien vers le chasseur)
				objet = _l->data(int(dx), int(dy));

				if((int) objet == 1) {
					return false;
				}
        		}
   		}

		// Le gardien prend la direction vers le chasseur
		float angle = atan(sx / sy);
		int rapport = (sy > 0) ? 0 : 180;
		int degres = int(angle * -180.0f / M_PI);
		_angle = degres + rapport;

		return true;
	}


	/*
	* Fonction qui permet, en mode patrouille uniquement, que le gardien regarde devant lui avant d'avancer pour éviter qu'il rentre dans un obstacle.
	* On regarde assez loin devant car les murs sont assez large.
	*/
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

/*
* La fonction patrouille correspond à la mise en action du gardien en mode patrouille. Ce mode est activé quand le gardien ne voit pas le chasseur.
* Tout d'abord, on vérifie si l'angle courant du gardien n'est pas un des points cardinaux (cela veut dire qu'il sort du mode attaque), si c'est le cas,
* alors on change l'angle en un des points cardinaux. Le mode patrouille consiste en un déplacement dans la direction du gardien jusqu'à qu'il rencontre un obstacle
* et change de direction aléatoirement (toujours une direction d'un des points cardinaux) ou qu'il rentre en mode attaque.
*/
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

/*
* La fonction attaque fonctionne pour le debut de la meme façon que la fonction find. Le gardien se déplace le long de la droite entre le gardien et le chasseur,
* jusqu'à qu'il atteigne le chasseur. Comme cette fonction est execute apres find, on sait qu'il n'y a pas d'obstacle entre le gardien et le chasseur, alors le gardien
* s'avance d'un pas sur la droite et ensuite on recalculera avec find dans update si le gardien peut encore avancer.
*/
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


	/*
	* Cette fonction est lance dans quand le gardien est touche par une boule de feu, alors le gardien perd de la vie et s'il n'a plus de vie, alors il reste au sol
	* signifiant qu'il est mort.
	*/
	void etreToucheParBdf() {
		hp -= 5;
		if(hp <= 0) rester_au_sol();
	}

};

#endif
