/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) { /*El código que hay áquí, introducelo donde está la llamada a la función*/
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
} 

typedef struct {
	unsigned short numBloquesDatos; /* Número de bloques de datos en el disp. /*Lo ponemos como short porque no hace falta que sea un ing. Ocupa 5 * 2^10*/
	char bmapai[5];
	char bmapab[640]; 
	unsigned int tamDispositivo; /* Tamaño total del disp. (en bytes) */ /*Numero de bloques * 2048 (tamaño de bloque) */
} superbloque;

typedef struct {
	char nombre[32]; /* Nombre del fichero/directorio asociado cuya longitud máxima es de 32 caracteres */
	unsigned int tamanyo; /* Tamaño actual del fichero en bytes */
	unsigned int bloqueIndirecto; /* array de punteros*/
} inodo;