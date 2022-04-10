
// NightFox 3D LIB - Funciones de DEBUG
// Requiere DevkitARM R30
// Codigo por NightFox
// http://www.nightfoxandco.com/
// Version 20111011 - BETA



// Includes devKitPro
#include <nds.h>

// Includes C++
#include <stdio.h>
#include <string.h>

// Includes propios
#include "n3_debug.h"
#include "n3_defines.h"





// Funcion N3_Error();
void N3_Error(u16 code, const char* text, u32 value) {

	consoleDemoInit();		// Inicializa la consola de texto
	consoleClear();			// Borra la pantalla
	setBrightness(3, 0);	// Restaura el brillo


	// Errores definidos
	switch (code) {

		// Fuera de rango.
		case 101:
			iprintf("%s\n", text);
			iprintf("is out of range (0 - %ld).\n", value);
			break;

		// Slot o ID en uso.
		case 102:
			iprintf("%s %ld\n", text, value);
			iprintf("is already in use.\n");
			break;

		// Tama�o Ilegal
		case 103:
			iprintf("Illegal %s size.\n", text);
			iprintf("Id: %ld.\n", value);
			break;

		// Memoria RAM agotada
		case 104:
			iprintf("Out of RAM.\n");
			iprintf("%ld bytes can't be allocated.\n", value);
			break;

		// Fichero no encontrado
		case 105:
			iprintf("File\n");
			iprintf("%s\n", text);
			iprintf("not found.\n");
			break;

		// Slot o ID sin datos, no usado.
		case 106:
			iprintf("%s %ld\n", text, value);
			iprintf("is empty.\n");
			break;

		// Memoria VRAM insuficiente
		case 107:
			iprintf("Out of VRAM.\n");
			iprintf("%ld bytes for %s\n", value, text);
			iprintf("can't be allocated.\n");
			break;

		// Slot o ID no esta en uso.
		case 108:
			iprintf("%s %ld\n", text, value);
			iprintf("is not in use.\n");
			break;

		// Error desconocido
		default:
			iprintf("Unknow error");
			break;

	}


	// Imprime el codigo de error
	iprintf("Error code %ld.\n", code);


	// Deten la ejecucion del programa
	while(1)swiWaitForVBlank();
}