/*
-------------------------------------------------

	NightFox's 3D Lib Template
	Cubo con texturas

	Requiere DevkitARM
	Requiere NightFox's Lib

	Codigo por NightFox
	http://www.nightfoxandco.com
	Inicio 25 de Febrero del 2011

	(c)2011 NightFox & Co.

-------------------------------------------------
*/





/*
-------------------------------------------------
	Includes
-------------------------------------------------
*/

// Includes c/c++
#include <stdio.h>

// Includes propietarios NDS
#include <nds.h>

// Includes librerias propias
#include <n3_lib.h>





/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

int main(int argc, char **argv) {

	consoleDemoInit();		// Inicializa la consola de texto
	consoleClear();			// Borra la pantalla
	setBrightness(3, 0);	// Restaura el brillo

	iprintf("\n Hello World!");

	// Bucle (repite para siempre)
	while(1) {

		swiWaitForVBlank();		// Espera al sincronismo vertical
	
	}

	return 0; 

}
