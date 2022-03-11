
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
#include "n3_vram.h"
#include "n3_defines.h"
#include "n3_system.h"
#include "n3_debug.h"





// Funcion N3_VramTexture();
void N3_VramTexture(u16 ram, u16 vram, bool keepframes) {

	// Verifica el rango de Slots en RAM
	if ((ram < 0) || (ram >= N3_TEX_GFX_SLOTS)) {
		N3_Error(101, "Texture RAM SLOT", (N3_TEX_GFX_SLOTS - 1));
	}

	// Verifica si el Slot en RAM contiene datos
	if (N3_TEX_GFX[ram].available) {
		N3_Error(106, "Texture RAM SLOT", ram);
	}

	// Verifica el rango de Slots en VRAM
	if ((vram < 0) || (vram >= N3_VRAM_TEX_SLOTS)) {
		N3_Error(101, "Texture VRAM SLOT", (N3_VRAM_TEX_SLOTS - 1));
	}

	// Verifica si el Slot de VRAM esta libre
	if (N3_TEXTURE[vram].inuse) {
		N3_Error(102, "Texture VRAM SLOT", vram);
	}

	// Variables de uso general
	s16 n = 0;				// General
	s16 id = 255;			// Id del posible bloque libre
	s16 last_reuse = 0;		// Nº del ultimo bloque reusable
	u32 gfxsize = 0;		// Tamaño de los datos que se copiaran
	u32 size = 0;			// Diferencia de tamaños entre bloque libre y datos
	bool organize = true;	// Se debe de reorganizar el array de bloques libres ?

	// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
	vramSetBankB(VRAM_B_LCD);

	// Auto calcula el tamaño de 1 frame
	N3_TEXTURE[vram].framesize = (N3_TEX_GFX[ram].width * N3_TEX_GFX[ram].height);
	// Auto calcula el ultimo frame de la animacion
	N3_TEXTURE[vram].lastframe = (((int)(N3_TEX_GFX[ram].size / N3_TEXTURE[vram].framesize)) - 1);
	N3_TEXTURE[vram].inuse = true;						// Slot ocupado

	// Calcula el tamaño de la textura a copiar segun si debes o no copiar todos los frames
	if (keepframes) {	// Si debes de mantener los frames en RAM, solo copia el primero
		gfxsize = N3_TEXTURE[vram].framesize;
	} else {			// Si no, copialos todos
		gfxsize = N3_TEX_GFX[ram].size;
	}

	// Actualiza la VRAM disponible
	N3_TEXVRAM.free -= gfxsize;

	// Si no hay suficiente VRAM, error
	if (N3_TEXVRAM.free < 0) {
		N3_Error(107, "Textures", gfxsize);
	}

	// Si hay que aprovechar algun bloque borrado... (tamaño identico, preferente)
	if (N3_TEXVRAM.deleted > 0) {
		// Busca un bloque vacio del tamaño identico
		for (n = 0; n < N3_TEXVRAM.deleted; n ++) {
			if (N3_TEXVRAM.size[n] == gfxsize) {		// Si el bloque tiene el tamaño suficiente
				id = n;		// Guarda la Id
				n = N3_TEXVRAM.deleted;	// y sal
			}
		}
		// Si no habia ningun bloque de tamaño identico, busca el mas parecido (produce fragmentacion)
		if (id == 255) {
			for (n = 0; n < N3_TEXVRAM.deleted; n ++) {
				if (N3_TEXVRAM.size[n] > gfxsize) {		// Si el bloque tiene el tamaño suficiente
					id = n;		// Guarda la Id
					n = N3_TEXVRAM.deleted;	// y sal
				}
			}
		}
	} 
	
	// Si hay algun bloque borrado libre del tamaño suficiente...
	if (id != 255) {

		// Transfiere la textura a la VRAM
		N3_DmaMemCopy((void*)N3_TEXVRAM.pos[id], N3_TEX_GFX[ram].buffer, gfxsize);
		// Guarda el puntero donde lo has almacenado
		N3_TEXTURE[vram].address = N3_TEXVRAM.pos[id];

		// Si no has usado todo el tamaño, deja constancia
		if (gfxsize < N3_TEXVRAM.size[id]) {

			// Calcula el tamaño del nuevo bloque libre
			size = N3_TEXVRAM.size[id] - gfxsize;
			// Actualiza los datos
			N3_TEXVRAM.pos[id] += gfxsize;			// Nueva direccion
			N3_TEXVRAM.size[id] = size;				// Nuevo tamaño
			N3_TEXVRAM.fragmented -= gfxsize;		// Actualiza el contador de VRAM fragmentada
			organize = false;						// No se debe de reorganizar el array de bloques

		} else {	// Si has usado todo el tamaño, deja constancia

			N3_TEXVRAM.fragmented -= N3_TEXVRAM.size[id];	// Actualiza el contador de VRAM fragmentada

		}

		// Se tiene que reorganizar el array de bloques libres ?
		if (organize) {
			last_reuse = (N3_TEXVRAM.deleted - 1);
			if (
			(last_reuse > 0)	// Si hay mas de un bloque borrado
			&&
			(id != last_reuse)	// Y no es la ultima posicion
			) {
				// Coloca los valores de la ultima posicion en esta
				N3_TEXVRAM.pos[id] = N3_TEXVRAM.pos[last_reuse];		// Nueva direccion
				N3_TEXVRAM.size[id] = N3_TEXVRAM.size[last_reuse];		// Nuevo tamaño
			}
			N3_TEXVRAM.deleted --;		// Actualiza el contador de bloques libres, borrando el ultimo registro
		}

	} else {	// Si no habia ningun bloque borrado o con el tamaño suficiente, colacalo al final de la VRAM ocupada

		// Actualiza la VRAM contigua disponible (mayor bloque libre al final)
		N3_TEXVRAM.inarow -= gfxsize;

		// Si no hay suficiente VRAM (contigua), error
		if (N3_TEXVRAM.inarow < 0) {
			N3_Error(107, "Textures", gfxsize);
		}

		// Transfiere el grafico a la VRAM
		N3_DmaMemCopy((void*)N3_TEXVRAM.next, N3_TEX_GFX[ram].buffer, gfxsize);
		
		// Guarda el puntero donde lo has almacenado
		N3_TEXTURE[vram].address = N3_TEXVRAM.next;
		// Guarda la direccion actual como la ultima usada
		N3_TEXVRAM.last = N3_TEXVRAM.next;
		// Calcula la siguiente posicion libre
		N3_TEXVRAM.next += gfxsize;

	}

	// Guarda los datos de la textura que se ha copiado a la VRAM.
	N3_TEXTURE[vram].size = gfxsize;					// Tamaño en bytes de los datos copiados
	N3_TEXTURE[vram].width = N3_TEX_GFX[ram].width;		// Alto (px)
	N3_TEXTURE[vram].height = N3_TEX_GFX[ram].height;	// Ancho (px)
	N3_TEXTURE[vram].ramid = ram;						// Slot RAM de origen
	N3_TEXTURE[vram].keepframes = keepframes;			// Debes guardar los frames en RAM o copiarlos a la VRAM?
	N3_TEXTURE[vram].frame = 0;							// Frame de la textura
	N3_TEXTURE[vram].newframe = 0;						// Actualizacion de la textura en VRAM;

	// Guarda el formato de la textura
	u16 x_size = N3_GetTextureSize(N3_TEX_GFX[ram].width);
	u16 y_size = N3_GetTextureSize(N3_TEX_GFX[ram].height);
	N3_TEXTURE[vram].format = (((N3_TEXTURE[vram].address >> 3) & 0xFFFF) | (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) | GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF);

	// Restaura el banco de VRAM en modo Textura
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

}



// Funcion N3_FreeTexture();
void N3_FreeTexture(u16 vram) {

	// Verifica si el Slot de VRAM esta libre
	if (!N3_TEXTURE[vram].inuse) {
		N3_Error(106, "Texture VRAM SLOT", vram);
	}

	// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
	vramSetBankB(VRAM_B_LCD);

	// Borra el Gfx de la VRAM (pon a 0 todos los Bytes)
	memset((void*)N3_TEXTURE[vram].address, 0, N3_TEXTURE[vram].size);

	// Actualiza la cantidad de VRAM disponible
	N3_TEXVRAM.free += N3_TEXTURE[vram].size;

	// Guarda la posicion y tamaño del bloque borrado para su reutilizacion
	N3_TEXVRAM.pos[N3_TEXVRAM.deleted] = N3_TEXTURE[vram].address;
	N3_TEXVRAM.size[N3_TEXVRAM.deleted] = N3_TEXTURE[vram].size;

	// Incrementa en contador de bloques borrados
	N3_TEXVRAM.deleted ++;

	// Incrementa el contador de memoria fragmentada
	N3_TEXVRAM.fragmented += N3_TEXTURE[vram].size;

	// Reinicia los datos de esta Id. de Texturas
	N3_TEXTURE[vram].size = 0;			// Tamaño en bytes
	N3_TEXTURE[vram].width = 0;			// Alto (px)
	N3_TEXTURE[vram].height = 0;		// Ancho (px)
	N3_TEXTURE[vram].address = 0;		// Puntero en VRAM
	N3_TEXTURE[vram].framesize = 0;		// Tamaño del frame (en bytes)
	N3_TEXTURE[vram].lastframe = 0;		// Ultimo frame
	N3_TEXTURE[vram].frame = 0;			// Frame de la textura
	N3_TEXTURE[vram].newframe = 0;		// Actualizacion de la textura en VRAM;
	N3_TEXTURE[vram].inuse = false;		// Marcalo como libre

	// Debes desfragmentar la VRAM
	if (N3_TEXVRAM.fragmented > (N3_TEXVRAM.inarow >> 1)) N3_VramTextureDefrag();

	// Restaura el banco de VRAM en modo Textura
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

}



// Funcion N3_VramTextureDefrag();
void N3_VramTextureDefrag(void) {

	// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
	vramSetBankB(VRAM_B_LCD);

	// Calcula la VRAM en uso y crea un buffer para guardarla
	u32 used_vram = ((131072 - N3_TEXVRAM.free) + 1);
	char* buffer;
	buffer = (char*) calloc (used_vram, sizeof(char));
	if (buffer == NULL) {		// Si no hay suficiente RAM libre
		N3_Error(104, NULL, used_vram);
	}

	char* address[N3_VRAM_TEX_SLOTS];	// Guarda la direccion en RAM
	u32 size[N3_VRAM_TEX_SLOTS];		// Guarda el tamaño
	u32 ram = 0;						// Puntero inicial de RAM
	u16 n = 0;							// Variable General
	u16 x_size = 0;		// Formato de la textura
	u16 y_size = 0;

	// Copia los datos de la VRAM a la RAM
	for (n = 0; n < N3_VRAM_TEX_SLOTS; n ++) {
		// Si esta en uso
		if (N3_TEXTURE[n].inuse) {
			// Copia el Gfx a la RAM
			address[n] = (buffer + ram);		// Calcula el puntero
			size[n] = N3_TEXTURE[n].size;		// Almacena el tamaño
			N3_DmaMemCopy(address[n], (void*)N3_TEXTURE[n].address, size[n]);	// Copialo a la VRAM
			ram += size[n];		// Siguiente posicion en RAM (relativa)
		}
	}

	// Inicializa la estructura de datos de la VRAM de Sprites
	N3_TEXVRAM.free = 131072;		// Memoria VRAM libre (128kb)
	N3_TEXVRAM.last = 0;			// Ultima posicion usada
	N3_TEXVRAM.deleted = 0;			// Ningun Gfx borrado
	N3_TEXVRAM.fragmented = 0;		// Memoria VRAM fragmentada
	N3_TEXVRAM.inarow = 131072;		// Memoria VRAM contigua
	for (n = 0; n < N3_VRAM_TEX_SLOTS; n ++) {
		N3_TEXVRAM.pos[n] = 0;		// Posicion en VRAM para reusar despues de un borrado
		N3_TEXVRAM.size[n] = 0;		// Tamaño del bloque libre para reusar
	}
	// Aplica la direccion de inicio de la VRAM
	N3_TEXVRAM.next = (0x06820000);

	// Ahora, copia de nuevo los datos a la VRAM, pero alineados
	u32 gfx_address = 0;
	for (n = 0; n < N3_VRAM_TEX_SLOTS; n ++) {
		// Si esta en uso
		if (N3_TEXTURE[n].inuse) {
			N3_DmaMemCopy((void*)N3_TEXVRAM.next, address[n], size[n]);		// Vuelve a colocar la textura en VRAM
			N3_TEXTURE[n].address = N3_TEXVRAM.next;		// Guarda la nueva posicion en VRAM
			N3_TEXVRAM.free -= size[n];			// Ram libre
			N3_TEXVRAM.inarow -= size[n];		// Ram libre en bloque
			N3_TEXVRAM.last = N3_TEXVRAM.next;	// Guarda la posicion como ultima usada
			N3_TEXVRAM.next += size[n];			// Y calcula la siguiente posicion a escribir
			// Para terminar, guarda el formato actualizado de la textura
			if (N3_TEXTURE[n].keepframes) {
				gfx_address = N3_TEXTURE[n].address;
			} else {
				gfx_address = (N3_TEXTURE[n].address + (N3_TEXTURE[n].framesize * N3_TEXTURE[n].frame));
			}
			x_size = N3_GetTextureSize(N3_TEXTURE[n].width);
			y_size = N3_GetTextureSize(N3_TEXTURE[n].height);
			N3_TEXTURE[n].format = (((gfx_address >> 3) & 0xFFFF) | (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) | GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF);
		}
	}

	// Vacia el buffer
	free(buffer);
	buffer = NULL;

	// Restaura el banco de VRAM en modo Textura
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

}



// Funcion N3_VramPalette();
void N3_VramPalette(u8 ram, u8 vram) {

	// Verifica el rango de Slots en RAM
	if ((ram < 0) || (ram >= N3_TEX_PAL_SLOTS)) {
		N3_Error(101, "Palette RAM SLOT", (N3_TEX_PAL_SLOTS - 1));
	}

	// Verifica si el Slot en RAM contiene datos
	if (N3_TEX_PAL[ram].available) {
		N3_Error(106, "Palette RAM SLOT", ram);
	}

	// Verifica el rango de Slots en VRAM
	if ((vram < 0) || (vram >= N3_VRAM_PAL_SLOTS)) {
		N3_Error(101, "Palette VRAM SLOT", (N3_VRAM_PAL_SLOTS - 1));
	}

	// Copia la paleta a la VRAM, segun el Slot
	u32 address = (0x06890000) + (vram << 9);	// Calcula donde guardaras la paleta
	vramSetBankF(VRAM_F_LCD);					// Bloquea el banco F para escribir las paletas
	N3_DmaMemCopy((void*)address, N3_TEX_PAL[ram].buffer, N3_TEX_PAL[ram].size);	// Copia la paleta al banco F
	vramSetBankF(VRAM_F_TEX_PALETTE);			// Banco F de la VRAM para paletas extendidas (Texturas)

	// Guarda los parametros
	N3_PALETTE[vram].inuse = true;			// Marca el SLOT en VRAM de paleta como en uso
	N3_PALETTE[vram].ramslot = ram;			// Guarda el slot de RAM donde esta la paleta original

	// Calcula el formato de la paleta
	address = (vram << 9);
	N3_PALETTE[vram].format = ((address >> 4) & 0x1FFF);

}



// Funcion N3_TextureFrame();
void N3_TextureFrame(u8 texture, u8 frame) {

	// Verifica si la textura esta en VRAM
	if (!N3_TEXTURE[texture].inuse) {
		N3_Error(108, "Texture VRAM slot ID", texture);
	}

	// Verifica el rango de frames de la textura
	if (frame > N3_TEXTURE[texture].lastframe) {
		N3_Error(101, "Texture frame", N3_TEXTURE[texture].lastframe);
	}

	// Si debes de copiar el nuevo frame desde la RAM a la VRAM...
	if (N3_TEXTURE[texture].keepframes) {

		// Marca para que se copie la nueva textura a la VRAM durante la actualizacion de los Sprites 3D
		N3_TEXTURE[texture].newframe = frame;
		N3_TEXVRAM.texupdate = true;

	} else {	// Si todos los frames ya estan en VRAM...

		// Calcula la direccion del Gfx del frame
		u32 gfx_address = (N3_TEXTURE[texture].address + (N3_TEXTURE[texture].framesize * frame));
		u16 x_size = N3_GetTextureSize(N3_TEXTURE[texture].width);
		u16 y_size = N3_GetTextureSize(N3_TEXTURE[texture].height);
		N3_TEXTURE[texture].format = (((gfx_address >> 3) & 0xFFFF) | (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) | GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF);
		// Guarda el numero de frame actual
		N3_TEXTURE[texture].frame = frame;
		N3_TEXTURE[texture].newframe = frame;

	}

}



// Funcion N3_UpdateTexturesVram();
void N3_UpdateTexturesVram(void) {

	// Variables
	u16 n = 0;				// Uso general
	char* source;			// Puntero de origen
	u32 destination = 0;	// Puntero de destino

	// Si hay texturas que actualizar...
	if (N3_TEXVRAM.texupdate) {

		// Si es necesario, actualiza las texturas de la RAM a la VRAM
		// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
		vramSetBankB(VRAM_B_LCD);

		// Busca los frames a actualizar
		for (n = 0; n < N3_VRAM_TEX_SLOTS; n ++) {
			// Si los frames adicionales estan en RAM y debe de cambiar de frame
			if (
				N3_TEXTURE[n].keepframes
				&&
				(N3_TEXTURE[n].frame != N3_TEXTURE[n].newframe)
				) {
				// Calcula el origen y destino del nuevo frame a copiar
				source = N3_TEX_GFX[N3_TEXTURE[n].ramid].buffer + (N3_TEXTURE[n].framesize * N3_TEXTURE[n].newframe);
				destination = N3_TEXTURE[n].address;
				// Copialo
				N3_DmaMemCopy((void*)destination, source, N3_TEXTURE[n].framesize);
				// Y actualiza el frame actual
				N3_TEXTURE[n].frame = N3_TEXTURE[n].newframe;
			}
		}

		// Restaura el banco de VRAM en modo Textura
		vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

		// Y marca que ninguna textura necesita ser actualizada
		N3_TEXVRAM.texupdate = false;

	}

}




