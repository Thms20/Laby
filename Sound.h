#ifndef SOUND_H
#define SOUND_H

#if defined (__APPLE__) || defined (UBUNTU)
/*
 *	Version portable pour Mac OS et Linux Ubuntu: on lance simplement le
 *	programme 'afplay / paplay' avec le bon volume et le bon fichier en argument!
 */

#include <stdio.h>
#include <stdlib.h>

class Sound {
	char filename [256];		// nom du fichier contenant le son à jouer.
	int	error;					// core d'erreur de 'system', non utilisé.
public:
	Sound (const char* name) { sprintf (filename, "%s", name); }
	// fait jouer le son.
	void play (float volume =1., float =0.) {
		char	cmd [512];

#ifdef __APPLE__
		sprintf (cmd, "afplay -v %g %s &", volume*5, filename);	// 5 semble être la valeur maximale.
#else
		sprintf (cmd, "paplay --volume=%d %s &", (int) (volume*65536), filename);
#endif
		error = system (cmd);
	}
};

#else

#ifdef _WIN32
#	include <windows.h>
#include <stdio.h>
#include <pthread.h>
#else
#include "fmod.hpp"
#include "fmod_errors.h"
#endif

#define MAX_SOUNDS	64

class Sound {
private:
#ifdef _WIN32
	char*	_sound;		// buffer contenant l'image du fichier.
#else
	static FMOD::System*	_system;
	static FMOD::Channel*	_channel;
	FMOD::Sound*			_sound;
#endif
	static int				_nsounds;	// compte d'occurrences.

	void init (void);
public:
	Sound (const char*);							// charge un fichier de sons.
	~Sound ();										// libère les échantillons.
	void play (float volume =1., float pan =0.);	// le fait jouer.
};
#endif
#endif
