/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */

#include "include/filesystem.h"		// Headers for the core functionality
#include "include/auxiliary.h"		// Headers for auxiliary functions
#include "include/metadata.h"		// Type and structure declaration of the file system
#include "include/crc.h"			// Headers for the CRC functionality
#include <string.h> 				// Header para utilizar memcpy

static superbloque sbloque;								// Estructura para almacenar el superbloque
static inodo inodos [MAX_INODOS];						// Estructura para almacenar los inodos
static unsigned int inodos_abierto [MAX_INODOS];		// Estructura auxiliar para controlar si los inodos están abiertos
static unsigned int punteros_lec_esc [MAX_INODOS];		// Estructura auxiliar para almacenar punteros de lectura y escritura
			
/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{	
	/* Se comprueba si el tamaño deseado excede los límites */
	if (deviceSize < MIN_FS_SIZE || deviceSize > MAX_FS_SIZE)
	{
		return -1;
	}
	sbloque.numInodos =  MAX_INODOS;
	/* Se comprueba el número de bloques que tendrá el dispositivo en función de su tamaño */
	if (deviceSize%BLOCK_SIZE != 0)
	{
		sbloque.numBloquesDatos = deviceSize/BLOCK_SIZE + 1;
	} else {
		sbloque.numBloquesDatos = deviceSize/BLOCK_SIZE;
	}
	/* Se ponen a 0 los mapas de bits */
	memset(sbloque.bmapai, 0, (sizeof(char)*5));
	memset(sbloque.bmapab, 0, (sizeof(char)*640));
	/* Se establece el tamaño del dispositivo */
	sbloque.tamDispositivo = sbloque.numBloquesDatos * BLOCK_SIZE;
	/* Buffer auxiliar para escribir el superbloque */
	char aux [BLOCK_SIZE];
	memcpy(&aux, &sbloque, sizeof(superbloque));
	/* Se escribe el superbloque en el disco */
	if(bwrite(DEVICE_IMAGE, 0, aux) == -1)
	{
		/* Se informa si se produce un problema en la escritura en el disco */
		return -1;
	}
	/* Se vacían los metadatos */
	memset(&sbloque, 0, sizeof(superbloque));
	memset(&inodos, 0, sizeof(inodo)*sbloque.numInodos);

	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{	
	/* Buffer auxiliar para leer */
	char aux [BLOCK_SIZE];
	/* Se lee el superbloque */
	if(bread(DEVICE_IMAGE, 0, aux) == -1)
	{
		/* Se informa si se produce un problema en la lectura del disco */
		return -1;
	}
	/* Se guarda en memoria */
	memcpy(&sbloque, &aux, sizeof(superbloque));
	/* Se borra aux */
	memset(&aux, 0, BLOCK_SIZE);
	/* Se lee el bloque que contiene los inodos */
	if(bread(DEVICE_IMAGE, 1, aux) == -1)
	{
		/* Se informa si se produce un problema en la lectura del disco */
		return -1;
	}
	/* Se guardan en memoria los inodos*/
	memcpy(&inodos, &aux, sizeof(inodo)*sbloque.numInodos);
	/* Se establece las estructuras auxiliares a 0 */
	memset(&inodos_abierto, 0, sizeof(unsigned int)*sbloque.numInodos);
	memset(&punteros_lec_esc, 0, sizeof(unsigned int)*sbloque.numInodos);
	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	/* Se comprueba si hay algún inodo abierto */
	for (int i = 0; i < sbloque.numInodos; ++i)
	{
		if (inodos_abierto[i] != 0)
		{
			return -1;
		}
	}
	/* Buffer auxiliar para escribir el superbloque */
	char aux [BLOCK_SIZE];
	/* Se copia el superbloque a aux */
	memcpy(&aux, &sbloque, sizeof(superbloque));
	/* Se escribe el superbloque en el disco */
	if(bwrite(DEVICE_IMAGE, 0, aux) == -1)
	{
		/* Se informa si se produce un problema en la escritura en el disco */
		return -1;
	}
	/* Se borra aux */
	memset(&aux, 0, BLOCK_SIZE);
	/* Se copian los inodos a aux */
	memcpy(&aux, &inodos, sizeof(inodo)*sbloque.numInodos);
	/* Se escriben los inodos en el disco */
	if(bwrite(DEVICE_IMAGE, 1, aux) == -1)
	{
		/* Se informa si se produce un problema en la escritura en el disco */
		return -1;
	}
	/* Se vacían los metadatos */
	memset(&sbloque, 0, sizeof(superbloque));
	memset(&inodos, 0, sizeof(inodo)*sbloque.numInodos);

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{
	/* Se comprueba si existe un archivo con ese nombre */
	int in = namei(fileName);
	if (in != -1)
	{
		return -1;
	}
	/* Se compruena si hay un inodo libre */
	int inodo = ialloc();
	if (inodo == -1)
	{
		/* Si no, se devuelve -2 */
		return -2;
	}
	/* Se comprueba si hay un bloque para el indice fichero */
	int bloquei = alloc();
	if (bloquei == -1)
	{
		/* Si no, se libera el inodo reservado y se devuelve -2 */
		ifree(inodo);
		return -2;
	}
	/* Se comprueba si hay un bloque para almacenar datos */
	int bloque1 = alloc();
	if (bloque1 == -1)
	{
		/* Si no, se liberan el inodo reservado y el bloque índice, y se devuelve -2 */
		ifree(inodo);
		bfree(bloquei);
		return -2;
	} 
	/* Si hay un inodo, un bloque índice y un bloque para datos disponibles, se crea el fichero */
	/* Se establece su tamaño */
	inodos[inodo].tamanyo = 0;
	/* Se le asigna el índice */	
	inodos[inodo].bloqueIndirecto = bloquei;
	/* Se pone el primer bloque en el índice */	
	uint16_t indice1 = (uint16_t) bloque1;
	char aux [BLOCK_SIZE];
	memcpy(&aux, &indice1, sizeof(uint16_t));
	/* Se escribe el índice en el bloque */	
	bwrite(DEVICE_IMAGE, META_BLOCKS + inodos[inodo].bloqueIndirecto, aux);
	/* Se le asigna el nombre */	
	strcpy(inodos[inodo].nombre, fileName);
	/* Se limpia el buffer para leer el primer bloque */
	memset(&aux, 0, BLOCK_SIZE);
	/* Se hace el CRC del primer bloque y se le asigna al inodo */
	bread(DEVICE_IMAGE, META_BLOCKS + bloque1 , aux);
	inodos[inodo].CRC = CRC32((const unsigned char *) aux, BLOCK_SIZE, 0);

	return 0;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName)
{
	/* Se comprueba si existe un archivo con ese nombre */
	int inodo = namei(fileName);
	if (inodo == -1)
	{
		return -1;
	} 
    /* Se comprueba si el fichero está cerrado */
    if (inodos_abierto[inodo] != 0)
    {
    	return -2;
    }
    /* Se liberan los bloques de datos */
    char aux [BLOCK_SIZE];
    bread(DEVICE_IMAGE, META_BLOCKS + inodos[inodo].bloqueIndirecto, aux);
    uint16_t indice[512];
    memcpy(&indice, &aux, sizeof(uint16_t)*INDEX_SIZE);
    for (int i = 0; i < INDEX_SIZE; ++i)
    {
    	if (indice[i] == 0)
    	{
    		break;
    	}
    	if (bfree(indice[i]) == -1)
    	{
    		return -2;
    	}
    }
    /* Se libera el bloque de indices*/
    if (bfree(inodos[inodo].bloqueIndirecto) == -1)
    {
        return -2;
    }
    /* Se libera el bloque de inodos */
    if (ifree(inodo) == -1)
    {
        return -2;
    }
    return 0;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{	
	/* Se comprueba si el nombre existe */
	int inodo = namei(fileName);
	if (inodo == -1)
	{	
		return -1;	
	} 
	/* Se comprueba la integridad del fichero */
	if (checkFile(fileName) != 0)
	{
		return -1;
	}
	/* Se comprueba que no este abierto */
	if (inodos_abierto[inodo] == 0)
	{
		/* Se marca como abierto */
		inodos_abierto[inodo] = 1;
		/* Se restauran sus punteros de lectura y escritura */
		punteros_lec_esc[inodo] = 0;

		return inodo;
	}
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	/* Se comprueba si el descriptor es correcto*/
	if ((fileDescriptor < 0) || (fileDescriptor > sbloque.numInodos))
	{
		return -1;
	} else {
		/* Y si ese archivo es un residuo */
		if (bitmap_getbit(sbloque.bmapai, fileDescriptor) == 0)
		{
			return -1;
		}
	}
	/* Se comprueba si ya estaba cerrado */
	if (inodos_abierto[fileDescriptor] == 0)
	{
		return -1;
	}
	/* Le asignamos el valor 0 para indicar que esta no esta abierto*/
	inodos_abierto[fileDescriptor] = 0;
	return 0;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	/* Se comprueba que el descriptor sea correcto */
	if ((fileDescriptor < 0) || (fileDescriptor >= sbloque.numInodos))
	{
		return -1;
	} else {
		/* Y que ese archivo no sea un residuo */
		if (bitmap_getbit(sbloque.bmapai, fileDescriptor) == 0)
		{
			return -1;
		}
	}

	/* Se comprueba que el numero de bytes a leer sea mayor que 0 */
	if(numBytes <= 0)
	{
		return -1;
	}
	/* Se comprueba si el fichero esta abierto*/
	if (inodos_abierto[fileDescriptor] == 0)
	{
		return -1;
	}
	int leidos = 0;
	/* Se lee del disco el índice del fichero */
	uint16_t indice [INDEX_SIZE];
	char aux [BLOCK_SIZE];
	memset(&indice, 0, (sizeof(uint16_t)*INDEX_SIZE));
	memset(&aux, 0, BLOCK_SIZE);
	bread(DEVICE_IMAGE, META_BLOCKS + inodos[fileDescriptor].bloqueIndirecto, aux);
	memcpy(&indice, &aux, (sizeof(uint16_t)*INDEX_SIZE));
	/* Si lo que se desea leer es mayor que lo que queda desde el puntero hasta el final del fichero */
	if (numBytes > (inodos[fileDescriptor].tamanyo - punteros_lec_esc[fileDescriptor]))
	{	
		/* Lo que leerá será lo que quede desde el puntero hasta el final */
		numBytes = (inodos[fileDescriptor].tamanyo - punteros_lec_esc[fileDescriptor]);
	}
	/* Mientras quede algo por leer */
	while((numBytes - leidos) != 0) 
	{
	    memset(&aux, 0, BLOCK_SIZE);
	    /* Se lee el bloque en el que esté el puntero */
	    bread(DEVICE_IMAGE, META_BLOCKS + indice[(int) punteros_lec_esc[fileDescriptor]/BLOCK_SIZE], aux);
	    /* Si queda por leer más de lo que queda del bloque */
	    if (numBytes-leidos > (BLOCK_SIZE-(punteros_lec_esc[fileDescriptor]%BLOCK_SIZE)))
	    {
	    	/* Se lee lo que queda del bloque en el que está el puntero */
	    	memcpy(&buffer + leidos, &aux + (punteros_lec_esc[fileDescriptor]%BLOCK_SIZE), (BLOCK_SIZE-(punteros_lec_esc[fileDescriptor]%BLOCK_SIZE)));
	    	/* Y se actualizan el número de bytes leídos y el puntero */
	    	leidos += (BLOCK_SIZE-(punteros_lec_esc[fileDescriptor]%BLOCK_SIZE));
	    	punteros_lec_esc[fileDescriptor] += (punteros_lec_esc[fileDescriptor]%BLOCK_SIZE);

	    } else {
	    	/* Se leen tantos bytes como quedan por leer en bloque en el que está el puntero */
	    	memcpy(&buffer + leidos, &aux + (punteros_lec_esc[fileDescriptor]%BLOCK_SIZE), numBytes - leidos);
	    	/* Y se actualizan el número de bytes leídos y el puntero */
	    	leidos += numBytes - leidos;
	    	punteros_lec_esc[fileDescriptor] += (numBytes - leidos);
	    }
	}
	/* Devolvemos el numero de bytes leidos*/
	return leidos; 
}


/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	//* Se comprueba que el descriptor sea correcto */
	if ((fileDescriptor < 0) || (fileDescriptor >= sbloque.numInodos))
	{
		return -1;
	} else {
		/* Y si ese archivo es un residuo */
		if (bitmap_getbit(sbloque.bmapai, fileDescriptor) == 0)
		{
			return -1;
		}
	}
	/* Se comprueba que el numero de bytes a leer sea mnayor que 0 */
	if(numBytes <= 0)
	{
		return -1;
	}
	/* Se comprueba si el fichero esta abierto*/
	if (inodos_abierto[fileDescriptor] == 0)
	{
		return -1;
	}
	int puntero_inicio = punteros_lec_esc[fileDescriptor];
	int escritos = 0;
	/* Se lee del disco el índice del fichero */
	uint16_t indice [INDEX_SIZE];
	char aux [BLOCK_SIZE];
	memset(&indice, 0, (sizeof(uint16_t)*INDEX_SIZE));
	memset(&aux, 0, BLOCK_SIZE);
	bread(DEVICE_IMAGE, META_BLOCKS + inodos[fileDescriptor].bloqueIndirecto, aux);
	memcpy(&indice, &aux, (sizeof(uint16_t)*INDEX_SIZE));
	/* Mientras quede por escribir */
	while((numBytes - escritos) > 0) 
	{
		memset(&aux, 0, BLOCK_SIZE);
		/* Se comprueba si se va a exceder el tamaño máximo de fichero */
		if (((int) punteros_lec_esc[fileDescriptor]) >= (MAX_FILE_SIZE))
		{
			break;
		} else {
			/* Se comprueba si hay que expandir el fichero */
			if (indice[(int) punteros_lec_esc[fileDescriptor]/BLOCK_SIZE] == 0)
			{
				/* Se comprueba si hay un bloque para expandir */
				int in = alloc();
				if ( in == -1)
				{
					break;
				} 
				/* Si lo hay, se expande */
				indice[(int) punteros_lec_esc[fileDescriptor]/BLOCK_SIZE] = in;
			}
		}
		/* Se lee el bloque en el que esté el puntero */
	    bread(DEVICE_IMAGE, META_BLOCKS + indice[(int) punteros_lec_esc[fileDescriptor]/BLOCK_SIZE] , aux);
		/* Si queda por escribir más de lo que queda de bloque */
	    if ((numBytes - escritos) > (BLOCK_SIZE-(punteros_lec_esc[fileDescriptor]%BLOCK_SIZE)))
	    {
	    	/* Se escribe lo que queda del bloque en el que está el puntero */
	    	memcpy(&aux + (punteros_lec_esc[fileDescriptor]%BLOCK_SIZE), &buffer + escritos, (BLOCK_SIZE-(punteros_lec_esc[fileDescriptor]%BLOCK_SIZE)));
	    	/* Y se actualizan el número de bytes escritos y el puntero */
	    	escritos += (BLOCK_SIZE-(punteros_lec_esc[fileDescriptor]%BLOCK_SIZE));
	    	punteros_lec_esc[fileDescriptor] += BLOCK_SIZE-punteros_lec_esc[fileDescriptor]%BLOCK_SIZE;
	    } else {
	    	/* Se escriben tantos bytes como quedan por escribir en el bloque en el que está el puntero */
	    	memcpy(&aux + (punteros_lec_esc[fileDescriptor]%BLOCK_SIZE), &buffer + escritos, numBytes - escritos);
	    	/* Y se actualizan el número de bytes leídos y el puntero */
	    	escritos += numBytes - escritos;
	    	punteros_lec_esc[fileDescriptor] += (numBytes - escritos);
	    }
	    /* Se escribe el bloque en el disco */
	    bwrite(DEVICE_IMAGE, META_BLOCKS + indice[(int) punteros_lec_esc[fileDescriptor]/BLOCK_SIZE], aux);
	}
	/* Escribimos el índice actualizado en el disco */
	memset(&aux, 0, BLOCK_SIZE);
	memcpy(&aux, &indice, (sizeof(uint16_t)*INDEX_SIZE));
	bwrite(DEVICE_IMAGE, META_BLOCKS + inodos[fileDescriptor].bloqueIndirecto, aux);
	/* Actualizamos el tamaño del fichero si es necesario */
	if (puntero_inicio + escritos > inodos[fileDescriptor].tamanyo)
	{
		inodos[fileDescriptor].tamanyo = puntero_inicio + escritos;
	}
	/* Se actualiza el CRC del archivo */
	uint32_t CRC = 0;
	/* Para ello, se recorre el índice */
	for (int i = 0; i < INDEX_SIZE; ++i)
	{
		if (indice[i] == 0)
		{
			break;
		}
		memset(&aux, 0, BLOCK_SIZE);
		bread(DEVICE_IMAGE, META_BLOCKS + indice[i], aux);
		inodos[fileDescriptor].CRC = CRC32((const unsigned char *) aux, BLOCK_SIZE, CRC);
		
	}
	/* Devolvemos el numero de bytes escritos*/
	return escritos; 
}


/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	//* Se comprueba que el descriptor sea correcto */
	if ((fileDescriptor < 0) || (fileDescriptor >= sbloque.numInodos))
	{
		return -1;
	} else {
		/* Y si ese archivo es un residuo */
		if (bitmap_getbit(sbloque.bmapai, fileDescriptor) == 0)
		{
			return -1;
		}
	}
	/* En función del parámetro whence */
	switch (whence)
	{	
		case FS_SEEK_BEGIN :
		/* Se establece el puntero de lectura-escritura al principio del fichero */
		punteros_lec_esc[fileDescriptor] = 0;
		break;

		case FS_SEEK_END :
		/* Se establece el puntero de lectura-escritura al final del fichero */
		punteros_lec_esc[fileDescriptor] = inodos[fileDescriptor].tamanyo;
		break;

		case FS_SEEK_CUR :
		/* Si el offset es positivo */
		if (offset >= 0)
		{
			/* Se comprueba si se traspasa el final del fichero */
			if ((punteros_lec_esc[fileDescriptor] + offset) > inodos[fileDescriptor].tamanyo)
			{
				return -1;
			} else {
				/* Se actualiza el puntero */
				punteros_lec_esc[fileDescriptor] += offset;
			}
		} else {
			/* Se comprueba si se traspasa el principio del fichero */
			if ((punteros_lec_esc[fileDescriptor] + offset) < 0)
			{
				return -1;
			} else {
				/* Se actualiza el puntero */
				punteros_lec_esc[fileDescriptor] += offset;
			}
		}
		break;

		default :
		/* Si el valor de whence no era válido, se devuelve -1 */
		return -1;
	}
	return 0;
}

/*
 * @brief 	Verifies the integrity of the file system metadata.
 * @return 	0 if the file system is correct, -1 if the file system is corrupted, -2 in case of error.
 */
int checkFS(void)
{
	return -2;
}

/*
 * @brief 	Verifies the integrity of a file.
 * @return 	0 if the file is correct, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile(char *fileName)
{
	/* Se comprueba si existe un archivo con ese nombre*/
	int inodo = namei(fileName);
	if (inodo == -1)
	{
		return -2;
	}
	/* Se lee del disco el índice del fichero */
	uint16_t indice [512];
	char aux [BLOCK_SIZE];
	memset(&indice, 0, sizeof(uint16_t)*INDEX_SIZE);
	memset(&aux, 0, BLOCK_SIZE);
	bread(DEVICE_IMAGE, META_BLOCKS + inodos[inodo].bloqueIndirecto, aux);
	memcpy(&indice, &aux, sizeof(uint16_t)*INDEX_SIZE);
	/* Se realiza el CRC del archivo */
	uint32_t CRC = 0;
	/* Para ello, se recorre el índice */
	for (int i = 0; i < INDEX_SIZE; ++i)
	{
		if (indice[i] == 0)
		{
			break;
		}
		memset(&aux, 0, BLOCK_SIZE);
		bread(DEVICE_IMAGE, META_BLOCKS + indice[i], aux);
		CRC = CRC32((const unsigned char *) aux, BLOCK_SIZE, CRC);
		
	}
	if (inodos[inodo].CRC != CRC)
	{
		return -1;
	}
	return 0;
}

/*=============================================
=            Funciones auxiliares             =
=============================================*/

/*
 * @brief 	Devuelve el primer inodo libre en el sistema de ficheros.
 * @return 	Número del primer inodo libre, -1 si no hay inodos libres.
 */
int ialloc(void)
{
	/* Se recorre el mapa de bits de inodos buscando uno libre */
	for (int i =0; i < sbloque.numInodos; ++i)
	{
		/* Cuando se encuentra el primero */
		if (bitmap_getbit(sbloque.bmapai, i) == 0)
		{	
			/* Se marca como en uso */
			bitmap_setbit(sbloque.bmapai, i, 1);
			/* Se vacía */
			memset(&inodos[i], 0, sizeof(inodo));
			/* Y se devuelve */
			return i;
		}
	}
	/* Si no se ha encontrado ninguno, se devuelve -1 */
	return -1;
}

/*
 * @brief 	Devuelve el primer bloque de datos libre en el sistema de ficheros
 * @return 	Número del primer bloque libre, -1 si no hay bloques libres.
 */
int alloc(void)
{
	/* Se recorre el mapa de bits de bloques de datos buscando uno libre */
	for (int i = 0; i < sbloque.numBloquesDatos; ++i)
	{
		/* Cuando se encuentra el primero */
		if (bitmap_getbit(sbloque.bmapab, i) == 0)
		{	
			/* Se marca como en uso */
			bitmap_setbit(sbloque.bmapab, i, 1);
			/* Se vacía */
			char aux[BLOCK_SIZE];
			memset(&aux, 0, BLOCK_SIZE);
			bwrite(DEVICE_IMAGE, META_BLOCKS + i , aux);
			/* Y se devuelve */
			return i;
		}
	}
	/* Si no se ha encontrado ninguno, se devuelve -1 */
	return -1;
}

/*
 * @brief 	Comprueba si existe un inodo con cierto nombre
 * @return 	El número del inodo, -1 si no existe un inodo con ese nombre.
 */
int namei(char *fileName)
{
	/* Se recorren los inodos */
	for (int i = 0; i < sbloque.numInodos; ++i)
	{
		/* Cuando se encuentre uno con el mismo nombre */
		if (strcmp(inodos[i].nombre, fileName) == 0)
		{	
			/* Se comprueba que no sea un residuo */
			if (bitmap_getbit(sbloque.bmapai, i) == 1)
			{
				/* Se devuelve su número */
				return i;
			}
		}
	}
	/* Si no hay ninguno con ese nombre, se devuelve -1 */
	return -1;
}

/*
 * @brief 	Libera un inodo
 * @return 	0 si se libera correctamente, -1 en otro caso.
 */
int ifree(int inodo)
{
	/* Se comprueba si el inodo existe */
	if (inodo >= sbloque.numInodos)
	{
		return -1;
	}
	/* Si existe, se libera */
	bitmap_setbit( sbloque.bmapai, inodo, 0);
	return 0;
}

/*
 * @brief 	Libera un bloque
 * @return 	0 si se libera correctamente, -1 en otro caso.
 */
int bfree(int bloque)
{
	/* Se comprueba si el bloque existe */
	if (bloque >= sbloque.numBloquesDatos)
	{
		return -1;
	}
	/* Si existe, se libera */
	bitmap_setbit( sbloque.bmapab, bloque, 0);
	return 0;
}