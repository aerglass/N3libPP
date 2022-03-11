/*
-------------------------------------------------

	NightFox's 3D Lib Example
	Tunel con texturas

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


// Velocidad de movimiento
#define speed 1


// Funciones
void Render3dWorld(s32 cam, s32 rot, s32 eye_x, s32 eye_y);



/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

int main(int argc, char **argv) {

	// Inicializa la consola de texto para el Debug
	consoleDemoInit();

	// Inicializa el systema de archivos
	N3_InitFilesystem("NITROFS");

	// Inicializa los buffers
	N3_InitBuffers();

	// Carga los archivos necesarios
	u8 n = 0;
	char file[64];
	for (n = 0; n < 5; n ++) {
		sprintf (file, "sample%d", n);
		N3_LoadTexture(file, n, 32, 32);
		N3_LoadPal(file, n);
	}

	// Inicializa el modo 3D
	N3_Set3D(1, 0);
	N3_Init3dEngine();

	// Transfiere las texturas y paletas a la VRAM
	for (n = 0; n < 5; n ++) {
		N3_VramTexture(n, n, true);
		N3_VramPalette(n, n);
	}

	// Variables
	u32 keys = 0;			// Keypad
	touchPosition touch;	// Touchpad
	s32 cam = 0;			// Posicion de la camara
	s32 rot = 0;			// Rotacion de la camara
	s32 eye_x = 128;		// Donde mira la camara
	s32 eye_y = 96;

	// Texto
	consoleClear();
	iprintf("\n Use Up & down to move,\n");
	iprintf(" left & right to rotate.\n");
	iprintf(" Use stylus to look arround");


	// *** Inicializa la niebla ***
	// La niebla afecta a todos los poligonos
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FOG);
	// Habilita la niebla
	glEnable(GL_FOG);
	// Define el factor de profundidad de la niebla
	glFogShift(3);
	// Color de la niebla
	glFogColor(0, 0, 0, 0);
	// Crea la tabla de densidad de la niebla
	s32 f = 0;
	for (f = 0; f < 31; f ++) {
		glFogDensity(f, (f << 2));
	}
	glFogDensity(31, 127);
	// Define a partir de que punto empieza la niebla
	glFogOffset(0x6000 + (127 << 4));


	// Bucle infinito
	while (1) {

		// Lee el teclado y touchpad
		scanKeys();
		keys = keysHeld();
		touchRead(&touch);

		// Adelante y atras
		if (keys & KEY_DOWN) cam += speed;
		if (keys & KEY_UP) cam -= speed;
		// Rotacion
		if (keys & KEY_RIGHT) rot += speed;
		if (keys & KEY_LEFT) rot -= speed;
		// Orientacion de la camara
		eye_x = 128;
		eye_y = 96;
		if (keys & KEY_TOUCH) {
			eye_x = touch.px;
			eye_y = (191 - touch.py);
		}

		// Renderiza el mundo en 3D
		Render3dWorld(cam, rot, eye_x, eye_y);

		// Espera al sincronismo
		swiWaitForVBlank();

	}

	// Devuelve 0
	return 0;

}



	// Dibuja la escena en 3d
	void Render3dWorld(s32 cam, s32 rot, s32 eye_x, s32 eye_y) {

		// Variables locales
		s32 a = 0;	// Control del bucle
		s32 b = 0;

		s32 x = 0;	// Coordenadas de dibujado
		s32 y = 0;
		s32 z = 0;

		s32 size = 64;	// Tamaño de las piezas
		s32 side = 12;

		// Vertices
		typedef struct {
			s32 x;
			s32 y;
			s32 z;
		} vertex_info;
		vertex_info vertex[8];

		// Primero de todo, guarda la matriz
		glPushMatrix();

		// Y posiciona la camara
		gluLookAtf32(	(128 << 4), (96 << 4), ((128 + cam) << 4),
						(eye_x << 4), (eye_y << 4), ((0 + cam) << 4),
						((0 + rot) << 4), (128 << 4), 0);



		///////////////////////////////
		// Dibuja el lateral derecho //
		///////////////////////////////

		// Inicializa
		y = 0;
		z = 0;

		for (a = 0; a < 4; a ++) {

			// Resetea la X
			x = 0;

			for (b = 0; b < 4; b ++) {

				// Calcula los vertices
				vertex[0].x = ((x + size) << 4);
				vertex[0].y = (y << 4);
				vertex[0].z = -((z + size) << 4);

				vertex[1].x = (x << 4);
				vertex[1].y = (y << 4);
				vertex[1].z = -((z + size) << 4);

				vertex[2].x = (x << 4);
				vertex[2].y = (y << 4);
				vertex[2].z = -(z << 4);

				vertex[3].x = ((x + size) << 4);
				vertex[3].y = (y << 4);
				vertex[3].z = -(z << 4);

				vertex[4].x = (((x + size) - side) << 4);
				vertex[4].y = ((y + side) << 4);
				vertex[4].z = -(((z + size) - side) << 4);

				vertex[5].x = ((x + side) << 4);
				vertex[5].y = ((y + side) << 4);
				vertex[5].z = -(((z + size) - side) << 4);

				vertex[6].x = ((x + side) << 4);
				vertex[6].y = ((y + side) << 4);
				vertex[6].z = -((z + side) << 4);

				vertex[7].x = (((x + size) - side) << 4);
				vertex[7].y = ((y + side) << 4);
				vertex[7].z = -((z + side) << 4);

				// Dibuja la cara superior [0]
				N3_TexturedQuad(	vertex[4].x, vertex[4].y, vertex[4].z,
									vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									vertex[7].x, vertex[7].y, vertex[7].z,
									0, 0);
				// Dibuja borde 3 - 0 [1]
				N3_TexturedQuad(	vertex[0].x, vertex[0].y, vertex[0].z,
									vertex[4].x, vertex[4].y, vertex[4].z,
									vertex[7].x, vertex[7].y, vertex[7].z,
									vertex[3].x, vertex[3].y, vertex[3].z,
									1, 1);
				// Dibuja borde 0 - 1 [2]
				N3_TexturedQuad(	vertex[0].x, vertex[0].y, vertex[0].z,
									vertex[1].x, vertex[1].y, vertex[1].z,
									vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[4].x, vertex[4].y, vertex[4].z,
									2, 2);
				// Dibuja borde 1 - 2 [3]
				N3_TexturedQuad(	vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[1].x, vertex[1].y, vertex[1].z,
									vertex[2].x, vertex[2].y, vertex[2].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									3, 3);
				// Dibuja borde 2 - 3 [4]
				N3_TexturedQuad(	vertex[7].x, vertex[7].y, vertex[7].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									vertex[2].x, vertex[2].y, vertex[2].z,
									vertex[3].x, vertex[3].y, vertex[3].z,
									4, 4);

				// Calcula el siguiente bloque
				x += size;

			}

			// Calcula la siguiente fila
			z += size;

		}



		/////////////////////////////////
		// Dibuja el lateral Izquierdo //
		/////////////////////////////////

		// Inicializa
		y = 191;
		z = 0;

		for (a = 0; a < 4; a ++) {

			// Resetea la X
			x = 0;

			for (b = 0; b < 4; b ++) {

				// Calcula los vertices
				vertex[0].x = ((x + size) << 4);
				vertex[0].y = (y << 4);
				vertex[0].z = -(z << 4);

				vertex[1].x = (x << 4);
				vertex[1].y = (y << 4);
				vertex[1].z = -(z << 4);

				vertex[2].x = (x << 4);
				vertex[2].y = (y << 4);
				vertex[2].z = -((z + size) << 4);

				vertex[3].x = ((x + size) << 4);
				vertex[3].y = (y << 4);
				vertex[3].z = -((z + size) << 4);

				vertex[4].x = (((x + size) - side) << 4);
				vertex[4].y = ((y - side) << 4);
				vertex[4].z = -((z + side) << 4);

				vertex[5].x = ((x + side) << 4);
				vertex[5].y = ((y - side) << 4);
				vertex[5].z = -((z + side) << 4);

				vertex[6].x = ((x + side) << 4);
				vertex[6].y = ((y - side) << 4);
				vertex[6].z = -(((z + size) - side) << 4);

				vertex[7].x = (((x + size) - side) << 4);
				vertex[7].y = ((y - side) << 4);
				vertex[7].z = -(((z + size) - side) << 4);

				// Dibuja la cara superior [0]
				N3_TexturedQuad(	vertex[4].x, vertex[4].y, vertex[4].z,
									vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									vertex[7].x, vertex[7].y, vertex[7].z,
									0, 0);
				// Dibuja borde 3 - 0 [1]
				N3_TexturedQuad(	vertex[0].x, vertex[0].y, vertex[0].z,
									vertex[4].x, vertex[4].y, vertex[4].z,
									vertex[7].x, vertex[7].y, vertex[7].z,
									vertex[3].x, vertex[3].y, vertex[3].z,
									1, 1);
				// Dibuja borde 0 - 1 [2]
				N3_TexturedQuad(	vertex[0].x, vertex[0].y, vertex[0].z,
									vertex[1].x, vertex[1].y, vertex[1].z,
									vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[4].x, vertex[4].y, vertex[4].z,
									2, 2);
				// Dibuja borde 1 - 2 [3]
				N3_TexturedQuad(	vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[1].x, vertex[1].y, vertex[1].z,
									vertex[2].x, vertex[2].y, vertex[2].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									3, 3);
				// Dibuja borde 2 - 3 [4]
				N3_TexturedQuad(	vertex[7].x, vertex[7].y, vertex[7].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									vertex[2].x, vertex[2].y, vertex[2].z,
									vertex[3].x, vertex[3].y, vertex[3].z,
									4, 4);

				// Calcula el siguiente bloque
				x += size;

			}

			// Calcula la siguiente fila
			z += size;

		}


		/////////////////////
		// Dibuja el techo //
		/////////////////////

		// Inicializa
		x = 255;
		z = 0;

		for (a = 0; a < 4; a ++) {

			// Resetea la Y
			y = 0;

			for (b = 0; b < 3; b ++) {

				// Calcula los vertices
				vertex[0].x = (x << 4);
				vertex[0].y = ((y + size) << 4);
				vertex[0].z = -(z << 4);

				vertex[1].x = (x << 4);
				vertex[1].y = ((y + size) << 4);
				vertex[1].z = -((z + size) << 4);

				vertex[2].x = (x << 4);
				vertex[2].y = (y << 4);
				vertex[2].z = -((z + size) << 4);

				vertex[3].x = (x << 4);
				vertex[3].y = (y << 4);
				vertex[3].z = -(z << 4);

				vertex[4].x = ((x - side) << 4);
				vertex[4].y = (((y + size) - side) << 4);
				vertex[4].z = -((z + side) << 4);

				vertex[5].x = ((x - side) << 4);
				vertex[5].y = (((y + size) - side) << 4);
				vertex[5].z = -(((z + size) - side) << 4);

				vertex[6].x = ((x - side) << 4);
				vertex[6].y = ((y + side) << 4);
				vertex[6].z = -(((z + size) - side) << 4);

				vertex[7].x = ((x - side) << 4);
				vertex[7].y = ((y + side) << 4);
				vertex[7].z = -((z + side) << 4);

				// Dibuja la cara superior [0]
				N3_TexturedQuad(	vertex[4].x, vertex[4].y, vertex[4].z,
									vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									vertex[7].x, vertex[7].y, vertex[7].z,
									0, 0);
				// Dibuja borde 3 - 0 [1]
				N3_TexturedQuad(	vertex[0].x, vertex[0].y, vertex[0].z,
									vertex[4].x, vertex[4].y, vertex[4].z,
									vertex[7].x, vertex[7].y, vertex[7].z,
									vertex[3].x, vertex[3].y, vertex[3].z,
									1, 1);
				// Dibuja borde 0 - 1 [2]
				N3_TexturedQuad(	vertex[0].x, vertex[0].y, vertex[0].z,
									vertex[1].x, vertex[1].y, vertex[1].z,
									vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[4].x, vertex[4].y, vertex[4].z,
									2, 2);
				// Dibuja borde 1 - 2 [3]
				N3_TexturedQuad(	vertex[5].x, vertex[5].y, vertex[5].z,
									vertex[1].x, vertex[1].y, vertex[1].z,
									vertex[2].x, vertex[2].y, vertex[2].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									3, 3);
				// Dibuja borde 2 - 3 [4]
				N3_TexturedQuad(	vertex[7].x, vertex[7].y, vertex[7].z,
									vertex[6].x, vertex[6].y, vertex[6].z,
									vertex[2].x, vertex[2].y, vertex[2].z,
									vertex[3].x, vertex[3].y, vertex[3].z,
									4, 4);

				// Calcula el siguiente bloque
				y += size;

			}

			// Calcula la siguiente fila
			z += size;

		}

		// Dibuja el suelo
		x = 0;
		z = 0;
		for (a = 0; a < 4; a ++) {
			y = 0;
			for (b = 0; b < 3; b ++) {
				// Dibuja el poligono
				N3_TexturedQuad((x << 4), ((y + size) << 4), -((z + size) << 4),
								(x << 4), ((y + size) << 4), -(z << 4),
								(x << 4), (y << 4), -(z << 4),
								(x << 4), (y << 4), -((z + size) << 4),
								0, 0);
				// Calcula el siguiente
				y += size;
			}
			// Calcula el siguiente
			z += size;
		}

		// Recupera la matriz
		glPopMatrix(1);

		// Manda la escena 3D a la pantalla
		glFlush(0);

	}
