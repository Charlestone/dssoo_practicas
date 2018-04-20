/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
} 

typedef struct {
unsigned int numeroMagico; /* Número mágico del superbloque */
unsigned int numBloquesMapaInodos; /* Número de bloques del mapa inodos */
unsigned int numBloquesMapaDatos; /* Número de bloques del mapa datos */
unsigned int numInodos; /* Número de inodos en el dispositivo */
unsigned int primerInodo; /* Número bloque del 1º inodo del disp. (inodo raíz) */
unsigned int numBloquesDatos; /* Número de bloques de datos en el disp. */
unsigned int primerBloqueDatos; /* Número de bloque del 1º bloque de datos */
unsigned int tamDispositivo; /* Tamaño total del disp. (en bytes) */
char relleno[PADDING_SB]; /* Campo de relleno (para completar un bloque) ???*/
} superbloque;

typedef struct {
unsigned int tipo; /* T_FICHERO o T_DIRECTORIO */
char nombre[32]; /* Nombre del fichero/directorio asociado cuya longitud máxima es de 32 caracteres */
unsigned int inodosContenidos[40]; /* tipo==dir: lista de los inodos del directorio ES POSBILE QUE NO HAGA FALTA PORQUE NO HAY DIRECTORIOS O QUE SE UTILICE SOLO EN EL RAIZ*/
unsigned int tamanyo; /* Tamaño actual del fichero en bytes */
unsigned int bloqueDirecto; /* Número del bloque directo */
unsigned int bloqueIndirecto; /* array de punteros*/
/* Es posible que hagan falta punteros para más bloques */
char relleno[PADDING_INODO]; /* Campo relleno para llenar un bloque ???*/
} inodo;

#define bitmap_inodos
#define bitmap_bloques