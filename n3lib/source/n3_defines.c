
// NightFox 3D LIB - Definiciones y variables globales
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
#include "n3_defines.h"




// Path del filesystem (Maximo 32 caracteres)
char N3_ROOTFOLDER[32];

// Define la estructura de datos de los buffers de texturas en RAM
N3_TYPE_TEX_GFX_INFO N3_TEX_GFX[N3_TEX_GFX_SLOTS];

// Define la estructura de datos de los buffers de paletas en RAM
N3_TYPE_TEX_PAL_INFO N3_TEX_PAL[N3_TEX_PAL_SLOTS];

// Define la estructura de control de la VRAM para texturas
N3_TYPE_TEXVRAM_INFO N3_TEXVRAM;

// Define la estructura de control de las Texturas cargadas en VRAM
N3_TYPE_TEXTURE_INFO N3_TEXTURE[N3_VRAM_TEX_SLOTS];

// Define la estructura de control de la VRAM para paletas
N3_TYPE_PALVRAM_INFO N3_PALETTE[N3_VRAM_PAL_SLOTS];




