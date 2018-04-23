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
#define MAX_INODOS 40			/* Número máximo de inodos activos que podrá tener el sistema de ficheros */
#define MAX_FS_SIZE 10485760 	/* Tamaño máximo del sistema de ficheros en bytes*/
#define MIN_FS_SIZE 51200 		/* Tamaño mínimo del sistema de ficheros en bytes */
#define META_BLOCKS 2 			/* Número de bloques que contienen metadatos en el dispositivo */

#include <stdint.h>				//Header para utilizar uint

/* Estructura para el superbloque. Los datos útiles ocupan 5216 bits, 652 bytes. El superbloque completo ocupa 2048 bytes (1 bloque) */
typedef struct {
	uint8_t numInodos; 			/* Número de inodos totales en el dispositivo */
	uint16_t numBloquesDatos; 	/* Número de bloques de datos totales en el disp. Lo ponemos como short porque no hace falta que sea un ing. Ocupa 5 * 2^10*/
	char bmapai[5]; 			//Mapa de bits de los inodos
	char bmapab[640]; 			//Mapa de bits de los bloques de datos
	uint32_t tamDispositivo; 	/* Tamaño total del disp. (en bytes)*/
	char relleno[1396]; 		//Relleno del bloque
} superbloque;

/* Estructura para cada inodo. Ocupa 336 bits, 42 bytes. */
typedef struct {
	char nombre[32]; 			/* Nombre del fichero/directorio asociado cuya longitud máxima es de 32 caracteres */
	uint32_t tamanyo; 			/* Tamaño actual del fichero en bytes */
	uint32_t CRC;				/* CRC para comprobar la integridad del fichero */
	uint16_t bloqueIndirecto; 	/* Indica el bloque en el que se encuentra el índice de bloques de datos que conforman el fichero*/
} inodo;
