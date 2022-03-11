/*
-------------------------------------------------

	NightFox's 3D Lib Example
	Cubo con texturas de varios frames

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


// Velocidad de rotacion
#define speed 2


// Funciones
void DrawCube(s16 fx, s16 fy, s16 fz, s16 fwidth, s16 fheight, s16 fdeep);



/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/

int main(int argc, char **argv) {

    // Variables
    s16 size = 64;	// Tamaño del cubo
    s16 x = (128 - (size >> 1));	// Posicion X
    s16 y = (96 + (size >> 1));		// Posicion Y
    s16 z = 0;		// Posicion Z
    s16 cx = 0;		// Centro X
    s16 cy = 0;		// Centro Y
    s16 cz = 0;		// Centro Z
    s16 rot_x = 75;	// Rotacion X
    s16 rot_y = 50;	// Rotacion Y
    s16 rot_z = 25;	// Rotacion Z
    u8 n = 0;		// Uso general
    s16 zoom = 128;	// Zoom
    u8 frame[6];	// Frame actual de cada cara

    u32 keys = 0;			// Teclas siempre presionadas
    u32 press = 0;			// Nueva pulsacion
    touchPosition touch;	// Touchpad

	// Inicializa la consola de texto para el Debug
	consoleDemoInit();

	// Inicializa el systema de archivos
	N3_InitFilesystem("NITROFS");

	// Inicializa los buffers
	N3_InitBuffers();

	// Carga los archivos necesarios
	N3_LoadTexture("frames", 0, 32, 32);	// Textura
	N3_LoadPal("frames", 0);				// Paleta

	// Inicializa el modo 3D en la pantalla inferior
	N3_Set3D(1, 0);		// Selecciona el modo 3D
	N3_Init3dEngine();	// Inicializa el OpenGL y la VRAM

	// Transfiere las texturas y paletas a la VRAM (solo el primer frame)
	for (n = 0; n <= 2; n ++) {
		N3_VramTexture(0, n, true);
		N3_VramPalette(0, n);
		N3_TextureFrame(n, n);
		frame[n] = n;
	}

	// Transfiere las texturas y paletas a la VRAM (todos los frames)
	for (n = 3; n <= 5; n ++) {
		N3_VramTexture(0, n, false);
		N3_VramPalette(0, n);
		N3_TextureFrame(n, n);
		frame[n] = n;
	}


	// Bucle infinito
	while (1) {

		// Lee el teclado y touchpad
		scanKeys();
		keys = keysHeld();
		press = keysDown();
		touchRead(&touch);

		// Guarda la matriz
		glPushMatrix();

		// Reposiciona la camara
		gluLookAtf32(	N3_RealToF32(128), N3_RealToF32(96), N3_RealToF32(zoom),
						N3_RealToF32(128), N3_RealToF32(96), N3_RealToF32(0),
						N3_RealToF32(0), N3_RealToF32(128), N3_RealToF32(0)
						);
		// Las funciones N3_RealToF32 y N3_RealToV16 sencillamente escalan el numero
		// a un valor que corresponde a las coordenadas reales de la pantalla

		// Si se ha tocado la pantalla, posiciona el cubo en esa posicion
		if (keys & KEY_TOUCH) {
			x = (touch.px - (size >> 1));
			y = ((192 - touch.py) + (size >> 1));
		}

		// Calcula la rotacion
		if (keys & KEY_DOWN) {
			rot_x += speed;
			if (rot_x > 512) rot_x -= 512;
		}
		if (keys & KEY_UP) {
			rot_x -= speed;
			if (rot_x < 0) rot_x += 512;
		}
		if (keys & KEY_LEFT) {
			rot_y += speed;
			if (rot_y > 512) rot_y -= 512;
		}
		if (keys & KEY_RIGHT) {
			rot_y -= speed;
			if (rot_y < 0) rot_y += 512;
		}
		if (keys & KEY_A) {
			rot_z += speed;
			if (rot_z > 512) rot_z -= 512;
		}
		if (keys & KEY_B) {
			rot_z -= speed;
			if (rot_z < 0) rot_z += 512;
		}
		if (keys & KEY_X) {
			zoom += speed;
			if (zoom > 256) zoom = 256;
		}
		if (keys & KEY_Y) {
			zoom -= speed;
			if (zoom < size) zoom = size;
		}

		// Cambia el frame de las caras (KEEPFRAMES = TRUE)
		if (press & KEY_R) {
			for (n = 0; n <= 2; n ++) {
				frame[n] ++;
				if (frame[n] > 5) frame[n] = 0;
				N3_TextureFrame(n, frame[n]);
			}
		}

		// Cambia el frame de las caras (KEEPFRAMES = FALSE)
		if (press & KEY_L) {
			for (n = 3; n <= 5; n ++) {
				frame[n] ++;
				if (frame[n] > 5) frame[n] = 0;
				N3_TextureFrame(n, frame[n]);
			}
		}

		// Calcula el centro
		cx = (x + (size >> 1));
		cy = (y - (size >> 1));
		cz = (z - (size >> 1));

		// Muevete al centro
		glTranslatef32(N3_RealToF32(cx), N3_RealToF32(cy), N3_RealToF32(cz));

		// Aplica la rotacion
		glRotateXi((rot_x << 6));
		glRotateYi((rot_y << 6));
		glRotateZi((rot_z << 6));

		// Vuelve a la posicion original
		glTranslatef32(N3_RealToF32(-cx), N3_RealToF32(-cy), N3_RealToF32(-cz));

		// Dibuja un cubo
		DrawCube(x, y, 0, size, size, size);

		// Recupera la matriz
		glPopMatrix(1);

		// Manda la escena 3D a la pantalla
		glFlush(0);

		// Espera al sincronismo vertical
		swiWaitForVBlank();

		// Actualiza, si es necesario los frames de animacion en VRAM
		N3_UpdateTexturesVram();

	}

	// Devuelve 0
	return 0;

}



// Dibuja un cubo
void DrawCube(s16 fx, s16 fy, s16 fz, s16 fwidth, s16 fheight, s16 fdeep) {

	// Convierte los valores "en pixeles" a V16
	s16 x = N3_RealToV16(fx);
	s16 y = N3_RealToV16(fy);
	s16 z = N3_RealToV16(fz);
	s16 width = N3_RealToV16(fwidth);
	s16 height = N3_RealToV16(fheight);
	s16 deep = N3_RealToV16(fdeep);

	// Dibuja la cara 1 (frente)
	N3_TexturedQuad(	x, y, z,
						x, (y - height), z,
						(x + width), (y - height), z,
						(x + width), y, z,
						0, 0);

	// Dibuja la cara 2 (arriba)
	N3_TexturedQuad(	(x + width), y, z,
						(x + width), y, (z - deep),
						x, y, (z - deep),
						x, y, z,
						1, 1);

	// Dibuja la cara 3 (derecha)
	N3_TexturedQuad(	(x + width), (y - height), z,
						(x + width), (y - height), (z - deep),
						(x + width), y, (z - deep),
						(x + width), y, z,
						2, 2);

	// Dibuja la cara 4 (abajo)
	N3_TexturedQuad(	x, (y - height), z,
						x, (y - height), (z - deep),
						(x + width), (y - height), (z - deep),
						(x + width), (y - height), z,
						3, 3);

	// Dibuja la cara 5 (izquierda)
	N3_TexturedQuad(	x, y, z,
						x, y, (z - deep),
						x, (y - height), (z - deep),
						x, (y - height), z,
						4, 4);

	// Dibuja la cara 6 (trasera)
	N3_TexturedQuad(	(x + width), y, (z - deep),
						(x + width), (y - height), (z - deep),
						x, (y - height), (z - deep),
						x, y, (z - deep),
						5, 5);

}
