
// NightFox 3D LIB - Funciones de dibujado de poligonos
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
#include "n3_draw.h"
#include "n3_defines.h"





// Funcion N3_SolidTriangle();
void N3_SolidTriangle(	s16 x1, s16 y1, s16 z1,		// Coordenadas Vertice 1 (arriba)
						s16 x2, s16 y2, s16 z2,		// Coordenadas Vertice 2 (abajo / izquierda)
						s16 x3, s16 y3, s16 z3,		// Coordenadas Vertice 3 (abajo / derecha)
						u8 r, u8 g, u8 b			// Color RGB (8 bits canal)
) {
	// Dibuja el triangulo
	glBegin(GL_TRIANGLE);
		// Vertice 1 - (arriba)
		glColor3b (r, g, b);
		glVertex3v16(x1, y1, z1);
		// Vertice 2 - (inferior izquierdo)
		glColor3b (r, g, b);
		glVertex3v16(x2, y2, z2);
		// Vertice 3 - (inferior derecho)
		glColor3b (r, g, b);
		glVertex3v16(x3, y3, z3);
}



// Funcion N3_TexturedTriangle();
void N3_TexturedTriangle(	s16 x1, s16 y1, s16 z1,		// Coordenadas Vertice 1 (arriba)
							s16 x2, s16 y2, s16 z2,		// Coordenadas Vertice 2 (abajo / izquierda)
							s16 x3, s16 y3, s16 z3,		// Coordenadas Vertice 3 (abajo / derecha)
							u16 tex, u8 pal				// Textura y paleta
) {
	// Aplica la textura
	GFX_PAL_FORMAT = N3_PALETTE[pal].format;
	GFX_TEX_FORMAT = N3_TEXTURE[tex].format;
	// Dibuja el triangulo
	glBegin(GL_TRIANGLE);
		// Vertice 1 - (arriba)
		glTexCoord2t16(inttot16(0), inttot16(0));
		glVertex3v16(x1, y1, z1);
		// Vertice 2 - (inferior izquierdo)
		glTexCoord2t16(inttot16(0), inttot16(N3_TEXTURE[tex].height));
		glVertex3v16(x2, y2, z2);
		// Vertice 3 - (inferior derecho)
		glTexCoord2t16(inttot16(N3_TEXTURE[tex].width), inttot16(N3_TEXTURE[tex].height));
		glVertex3v16(x3, y3, z3);
}



// Funcion N3_SolidQuad();
void N3_SolidQuad(	s16 x1, s16 y1, s16 z1,		// Coordenadas Vertice 1 (arriba / izquierda)
					s16 x2, s16 y2, s16 z2,		// Coordenadas Vertice 2 (abajo / izquierda)
					s16 x3, s16 y3, s16 z3,		// Coordenadas Vertice 3 (abajo / derecha)
					s16 x4, s16 y4, s16 z4,		// Coordenadas Vertice 4 (arriba / derecha)
					u8 r, u8 g, u8 b			// Color RGB (8 bits canal)
) {
	// Dibuja el cuadrado
	glBegin(GL_QUAD);
		// Vertice 1 - (superior izquierdo)
		glColor3b (r, g, b);
		glVertex3v16(x1, y1, z1);
		// Vertice 2 - (inferior izquierdo)
		glColor3b (r, g, b);
		glVertex3v16(x2, y2, z2);
		// Vertice 3 - (inferior derecho)
		glColor3b (r, g, b);
		glVertex3v16(x3, y3, z3);
		// Vertice 4 - (superior derecho)
		glColor3b (r, g, b);
		glVertex3v16(x4, y4, z4);
}



// Funcion N3_TexturedQuad();
void N3_TexturedQuad(	s16 x1, s16 y1, s16 z1,		// Coordenadas Vertice 1 (arriba / izquierda)
						s16 x2, s16 y2, s16 z2,		// Coordenadas Vertice 2 (abajo / izquierda)
						s16 x3, s16 y3, s16 z3,		// Coordenadas Vertice 3 (abajo / derecha)
						s16 x4, s16 y4, s16 z4,		// Coordenadas Vertice 4 (arriba / derecha)
						u16 tex, u8 pal				// Textura y paleta
) {
	// Aplica la textura
	GFX_PAL_FORMAT = N3_PALETTE[pal].format;
	GFX_TEX_FORMAT = N3_TEXTURE[tex].format;
	// Dibuja el cuadrado
	glBegin(GL_QUAD);
		// Vertice 1 - (superior izquierdo)
		glTexCoord2t16(inttot16(0), inttot16(0));
		glVertex3v16(x1, y1, z1);
		// Vertice 2 - (inferior izquierdo)
		glTexCoord2t16(inttot16(0), inttot16(N3_TEXTURE[tex].height));
		glVertex3v16(x2, y2, z2);
		// Vertice 3 - (inferior derecho)
		glTexCoord2t16(inttot16(N3_TEXTURE[tex].width), inttot16(N3_TEXTURE[tex].height));
		glVertex3v16(x3, y3, z3);
		// Vertice 4 - (superior derecho)
		glTexCoord2t16(inttot16(N3_TEXTURE[tex].width), inttot16(0));
		glVertex3v16(x4, y4, z4);
}




