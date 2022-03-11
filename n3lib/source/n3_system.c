
// NightFox 3D LIB - Funciones de sistema
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
#include "n3_system.h"
#include "n3_defines.h"





// Funcion N3_Set3D();
void N3_Set3D(u8 screen, u8 mode) {

	//////////////////////////////////////////////
	// Especifica la pantalla de destino del 3D //
	//////////////////////////////////////////////

	// Especifica en que pantalla estara el main engine (unico que puede usar 3D)
	if (screen == 0) {
		lcdMainOnTop();
	} else {
		lcdMainOnBottom();
	}


	///////////////////////////
	// Selecciona el modo 3D //
	///////////////////////////

	// Selecciona modo 3D que usaremos
	switch (mode) {
		case 0:		
			videoSetMode(MODE_0_3D);
			break;
		case 2:		
			videoSetMode(MODE_2_3D);
			break;
		case 5:
			videoSetMode(MODE_5_3D);
			break;
	}

}



// Funcion N3_Init3dEngine();
void N3_Init3dEngine(void) {

	// Variables
	u32 n = 0;

	///////////////////////
	// Configura la VRAM //
	///////////////////////

	// Inicializa la estructura de datos de la gestion de la VRAM de texturas
	N3_TEXVRAM.free = 131072;		// Memoria VRAM libre (128kb)
	N3_TEXVRAM.last = 0;			// Ultima posicion usada
	N3_TEXVRAM.deleted = 0;			// Ningun Gfx borrado
	N3_TEXVRAM.fragmented = 0;		// Memoria VRAM fragmentada
	N3_TEXVRAM.inarow = 131072;		// Memoria VRAM contigua
	N3_TEXVRAM.texupdate = false;	// Debe ser actualizada alguna textura?
	for (n = 0; n < N3_VRAM_TEX_SLOTS; n ++) {
		N3_TEXVRAM.pos[n] = 0;		// Posicion en VRAM para reusar despues de un borrado
		N3_TEXVRAM.size[n] = 0;		// Tamaño del bloque libre para reusar
	}

	// Inicializa la estructura de datos de las Texturas en VRAM
	for (n = 0; n < N3_VRAM_TEX_SLOTS; n ++) {
		N3_TEXTURE[n].size = 0;				// Tamaño (en bytes) de la Textura
		N3_TEXTURE[n].width = 0;			// Ancho de la textura
		N3_TEXTURE[n].height = 0;			// Altura de la textura
		N3_TEXTURE[n].address = 0;			// Posicion en la VRAM
		N3_TEXTURE[n].ramid = 0;			// Numero de Slot en RAM del que provienes
		N3_TEXTURE[n].framesize = 0;		// Tamaño del frame (en bytes)
		N3_TEXTURE[n].lastframe = 0;		// Ultimo frame
		N3_TEXTURE[n].format = 0;			// Formato de textura
		N3_TEXTURE[n].frame = 0;			// Frame actual de la textura
		N3_TEXTURE[n].newframe = 0;			// Actualizacion de la textura en VRAM;
		N3_TEXTURE[n].keepframes = false;	// Si es una textura animada, debes de mantener los frames en RAM ?
		N3_TEXTURE[n].inuse = false;		// Disponibilidat del Slot;
	}

	// Inicializa la estructura de datos de las Paletas en VRAM
	for (n = 0; n < N3_VRAM_PAL_SLOTS; n ++) {
		N3_PALETTE[n].inuse = false;	// Flag de slot en uso
		N3_PALETTE[n].ramslot = 0;		// Slot en ram del que proviene la paleta
		N3_PALETTE[n].format = 0;		// Formato de la paleta
	}

	// VRAM para TEXTURAS en el Banco B
	vramSetBankB(VRAM_B_LCD);					// Bloquea el banco para la escritura
	memset((void*)0x06820000, 0, 131072);		// Borra el contenido del banco B
	N3_TEXVRAM.next = (0x06820000);				// Guarda la primera posicion de VRAM para Gfx
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

	// VRAM para PALETAS
	vramSetBankF(VRAM_F_LCD);					// Bloquea el banco para la escritura
	memset((void*)0x06890000, 0, 16384);		// Borra el contenido del banco F
	vramSetBankF(VRAM_F_TEX_PALETTE);			// Banco F de la VRAM para paletas extendidas (Texturas) (16kb)


	//////////////////////////
	// Inicializa el OpenGL //
	//////////////////////////

	// Inicializa el OpenGL de Libnds
	glInit();

	// Define el tamaño de la ventana 3D (toda la pantalla)
	glViewport(0, 0, 255, 191);

	// Configura la matriz de proyeccion
	glMatrixMode(GL_PROJECTION);	// Selecciona la matriz
	glLoadIdentity();

	// Configura la visual
	gluPerspective(70, (256.0 / 192.0), 0.1, 40);		// Perspectiva

	// Posiciona la camara por defecto
	gluLookAtf32(	N3_RealToF32(0), N3_RealToF32(0), N3_RealToF32(128),	// Posicion de la camara
					N3_RealToF32(0), N3_RealToF32(0), N3_RealToF32(0),		// Punto hacia el que miras
					N3_RealToF32(0), N3_RealToF32(128), N3_RealToF32(0)		// Altura de la camara
					);

	// Configura la matriz de visualizacion de modelos
	glMatrixMode(GL_MODELVIEW);		// Selecciona la matriz
	glLoadIdentity();				// Y reseteala

	// Por defecto, todos los poligonos son opacos
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

	// Configura el fondo
	glClearColor(0, 0, 0, 0);		// Fondo transparente
	glClearDepth(0x7FFF);			// Define la distancia de vision

	// Configura la iluminacion global mediante el color del poligono
	glColor(RGB15(31, 31, 31));

	// Habilita las texturas
	glEnable(GL_TEXTURE_2D | GL_BLEND);

	// Habilita la capa de dibujado
	REG_DISPCNT |= (DISPLAY_BG0_ACTIVE);

}



// Funcion N3_GetTextureSize();
u16 N3_GetTextureSize(u16 textel) {

	// Variables
	u16 size = 0;

	// Devuelve el tamaño del textel, segun su base2
	switch (textel) {
		case 8:
			size = 0;
			break;
		case 16:
			size = 1;
			break;
		case 32:
			size = 2;
			break;
		case 64:
			size = 3;
			break;
		case 128:
			size = 4;
			break;
		case 256:
			size = 5;
			break;
		case 512:
			size = 6;
			break;
		case 1024:
			size = 7;
			break;
		default:
			size = 255;
			break;
	}

	// Devuelve el valor
	return size;

}



// Funcion N3_3dLayer();
void N3_3dLayer(u8 layer) {

	// Resetea los BITS de control de prioridad en todos los fondos
	REG_BG0CNT &= 0xfffc;		// Pon a 0 los bits 0 y 1 del registro
	REG_BG1CNT &= 0xfffc;
	REG_BG2CNT &= 0xfffc;
	REG_BG3CNT &= 0xfffc;

	// Reordena todas las capas segun la solicitud
	switch (layer) {
		case 0:		// 3D Sprites en la capa 0
			REG_BG0CNT += BG_PRIORITY_0;
			REG_BG1CNT += BG_PRIORITY_1;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 1:		// 3D Sprites en la capa 1
			REG_BG0CNT += BG_PRIORITY_1;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 2:		// 3D Sprites en la capa 2
			REG_BG0CNT += BG_PRIORITY_2;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_1;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 3:		// 3D Sprites en la capa 3
			REG_BG0CNT += BG_PRIORITY_3;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_1;
			REG_BG3CNT += BG_PRIORITY_2;
			break;
		default:	// 3D Sprites en la capa 0
			REG_BG0CNT += BG_PRIORITY_0;
			REG_BG1CNT += BG_PRIORITY_1;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
	}

}



// Funcion N3_DmaMemCopy();
void N3_DmaMemCopy(void* destination, const void* source, u32 size) {

	// Funcion basada en la documentacion de Coranac
	// http://www.coranac.com/2009/05/dma-vs-arm9-fight/

	// Datos de origen y destino
	u32 src = (u32)source;
	u32 dst = (u32)destination;

	// Verifica si los datos estan correctamente alineados
	if ((src | dst) & 1) {

		// No estan alineados para un DMA copy
		// Se realiza un copia con el memcpy();
		memcpy(destination, source, size);

	} else {

		// Estan alineados correctamente

		// Espera a que el canal 3 de DMA este libre
		while (dmaBusy(3));

		// Manda el cache a la memoria
		DC_FlushRange(source, size);

		// Dependiendo de la alineacion de datos, selecciona el metodo de copia
		if ((src | dst | size) & 3) {
			// Copia de 16 bits
			 dmaCopyHalfWords(3, source, destination, size);
		} else {
			// Copia de 32 bits
			dmaCopyWords(3, source, destination, size);
		}

		// Evita que el destino sea almacenado en cache
		DC_InvalidateRange(destination, size);

	}

}




