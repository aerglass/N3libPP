
// NightFox 3D LIB - Funciones de sistema
// Requiere DevkitARM R30
// Codigo por NightFox
// http://www.nightfoxandco.com/
// Version 20111011 - BETA



// Includes devKitPro
#include <nds.h>
#include <filesystem.h>
#include <fat.h>

// Includes C++
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Includes propios
#include "n3_load.h"
#include "n3_defines.h"
#include "n3_debug.h"
#include "n3_system.h"






// Funcion N3_InitBuffers();
void N3_InitBuffers(void) {

	// Variables
	u32 n = 0;

	// Inicializa los buffers de texturas
	for (n = 0; n < N3_TEX_GFX_SLOTS; n ++) {
		N3_TEX_GFX[n].size = 0;
		N3_TEX_GFX[n].width = 0;
		N3_TEX_GFX[n].height = 0;
		N3_TEX_GFX[n].available = true;
		N3_TEX_GFX[n].buffer = NULL;
	}

	// Inicializa los buffers de paletas
	for (n = 0; n < N3_TEX_PAL_SLOTS; n ++) {
		N3_TEX_PAL[n].size = 0;
		N3_TEX_PAL[n].available = true;
		N3_TEX_PAL[n].buffer = NULL;
	}

}



// Funcion N3_ResetBuffers();
void N3_ResetBuffers(void) {

	// Variables
	u32 n = 0;

	// Vacia los buffers de texturas
	for (n = 0; n < N3_TEX_GFX_SLOTS; n ++) {
		free(N3_TEX_GFX[n].buffer);
	}

	// Vacia los buffers de paletas
	for (n = 0; n < N3_TEX_PAL_SLOTS; n ++) {
		free (N3_TEX_PAL[n].buffer);
	}

	// Inicializa las estructuras de datos
	N3_InitBuffers();

}



// Funcion N3_RootPath();
void N3_RootPath(const char* path) {

	if (strcmp(path, "NITROFS") == 0) {	// Si se debe iniciar el modo NitroFS y FAT

		// Define NitroFS como la carpeta inicial
		sprintf(N3_ROOTFOLDER, "%s", "");

	} else {	// Si se debe iniciar solo la FAT

		// Define la carpeta inicial de la FAT
		sprintf(N3_ROOTFOLDER, "%s", path);

	}

}



// Funcion N3_InitFilesystem();
void N3_InitFilesystem(const char* path) {

	if (strcmp(path, "NITROFS") == 0) {	// Si se debe iniciar el modo NitroFS y FAT

		// Define NitroFS como la carpeta inicial
		sprintf(N3_ROOTFOLDER, "%s", "");
		// Intenta inicializar NitroFS
		//i(nitroFSInit(){ //old legacy code that does not longer work.
		if(nitroFSInit(NULL)) {
		} else {
			// NitroFS ok
			// Si es correcto, cambia al ROOT del NitroFS
			chdir("nitro:/");
			// Fallo. Deten el programa
			consoleDemoInit();	// Inicializa la consola de texto
			iprintf("NitroFS Init Error.\n");
			iprintf("Abnormal termination.\n\n");
			iprintf("Check if your flashcard is\n");
			iprintf("Argv compatible.\n");
			iprintf("If not, try to launch your ROM\n");
			iprintf("using Homebrew Menu.\n\n");
			iprintf("http://sourceforge.net/projects/devkitpro/files/hbmenu/");
			// Bucle infinito. Fin del programa
			while(1) {
				swiWaitForVBlank();
			}
		}

	} else {	// Si se debe iniciar solo la FAT

		// Define la carpeta inicial de la FAT
		sprintf(N3_ROOTFOLDER, "%s", path);
		// Intenta inicializar la FAT
		if (fatInitDefault()) {
			// Si es correcto, cambia al ROOT del FAT
			chdir("fat:/");
		} else {
			// Fallo. Deten el programa
			consoleDemoInit();	// Inicializa la consola de texto
			iprintf("Fat Init Error.\n");
			iprintf("Abnormal termination.\n\n");
			iprintf("Check if your flashcard is\n");
			iprintf("DLDI compatible and your ROM\n");
			iprintf("is correctly patched.\n");
			// Bucle infinito. Fin del programa
			while(1) {
				swiWaitForVBlank();
			}
		}

	}

}



// Funcion N3_LoadTexture();
void N3_LoadTexture(const char* file, u16 id,  u16 width, u16 height) {

	// Verifica el rango de Slots
	if ((id < 0) || (id >= N3_TEX_GFX_SLOTS)) {
		N3_Error(101, "Texture RAM SLOT", (N3_TEX_GFX_SLOTS - 1));
	}

	// Verifica si el Slot esta libre
	if (!N3_TEX_GFX[id].available) {
		N3_Error(102, "Texture RAM SLOT", id);
	}

	// Verifica el tama�o de la textura
	if ((N3_GetTextureSize(width) == 255) || (N3_GetTextureSize(height) == 255)) {
		N3_Error(103, "Texture", id);
	}

	// Vacia el buffer
	free(N3_TEX_GFX[id].buffer);
	N3_TEX_GFX[id].buffer = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .IMG
	sprintf(filename, "%s/%s.img", N3_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tama�o del archivo
		fseek(file_id, 0, SEEK_END);
		N3_TEX_GFX[id].size = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		N3_TEX_GFX[id].buffer = (char*) calloc (N3_TEX_GFX[id].size, sizeof(char));
		// Si no hay suficiente RAM libre
		if (N3_TEX_GFX[id].buffer == NULL) {
			N3_Error(104, NULL, N3_TEX_GFX[id].size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(N3_TEX_GFX[id].buffer, 1, N3_TEX_GFX[id].size, file_id);
	} else {	// Si el archivo no existe...
		N3_Error(105, filename, 0);
	}
	// Cierra el archivo
	fclose(file_id);

	// Guarda las medidas de la textura
	N3_TEX_GFX[id].width = width;		// Ancho
	N3_TEX_GFX[id].height = height;		// Alto

	// Y marca esta ID como usada
	N3_TEX_GFX[id].available = false;

}



// Funcion N3_UnloadTexture();
void N3_UnloadTexture(u16 id) {

	// Verifica el rango de Slots
	if ((id < 0) || (id >= N3_TEX_GFX_SLOTS)) {
		N3_Error(101, "Texture RAM SLOT", (N3_TEX_GFX_SLOTS - 1));
	}

	// Vacia el buffer
	free(N3_TEX_GFX[id].buffer);
	N3_TEX_GFX[id].buffer = NULL;

	// Resetea los datos
	N3_TEX_GFX[id].width = 0;		// Ancho
	N3_TEX_GFX[id].height = 0;		// Alto
	N3_TEX_GFX[id].size = 0;		// Tama�o en bytes

	// Y marca esta ID como libre
	N3_TEX_GFX[id].available = true;

}



// Funcion N3_LoadPal();
void N3_LoadPal(const char* file, u16 id) {

	// Verifica el rango de Slots
	if ((id < 0) || (id >= N3_TEX_PAL_SLOTS)) {
		N3_Error(101, "Palette RAM SLOT", (N3_TEX_PAL_SLOTS - 1));
	}

	// Verifica si el Slot esta libre
	if (!N3_TEX_PAL[id].available) {
		N3_Error(102, "Palette RAM SLOT", id);
	}

	// Vacia el buffer
	free(N3_TEX_PAL[id].buffer);
	N3_TEX_PAL[id].buffer = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .PAL
	sprintf(filename, "%s/%s.pal", N3_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tama�o del archivo
		fseek(file_id, 0, SEEK_END);
		N3_TEX_PAL[id].size = ftell(file_id);
		rewind(file_id);
		// Si el tama�o es inferior a 512, ajustalo
		if (N3_TEX_PAL[id].size < 512) N3_TEX_PAL[id].size = 512;
		// Reserva el espacio en RAM
		N3_TEX_PAL[id].buffer = (char*) calloc (N3_TEX_PAL[id].size, sizeof(char));
		// Si no hay suficiente RAM libre
		if (N3_TEX_PAL[id].buffer == NULL) {
			N3_Error(104, NULL, N3_TEX_PAL[id].size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(N3_TEX_PAL[id].buffer, 1, N3_TEX_PAL[id].size, file_id);
	} else {	// Si el archivo no existe...
		N3_Error(105, filename, 0);
	}
	// Cierra el archivo
	fclose(file_id);

	// Y marca esta ID como usada
	N3_TEX_PAL[id].available = false;

}



// Funcion N3_UnloadPal();
void N3_UnloadPal(u16 id) {

	// Verifica el rango de Slots
	if ((id < 0) || (id >= N3_TEX_PAL_SLOTS)) {
		N3_Error(101, "Palette RAM SLOT", (N3_TEX_PAL_SLOTS - 1));
	}

	// Vacia el buffer
	free(N3_TEX_PAL[id].buffer);
	N3_TEX_PAL[id].buffer = NULL;

	// Resetea los datos
	N3_TEX_PAL[id].size = 0;

	// Y marca esta ID como libre
	N3_TEX_PAL[id].available = true;

}




