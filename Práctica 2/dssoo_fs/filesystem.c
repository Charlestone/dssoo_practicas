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

static superbloque sbloque;						// Estructura para almacenar el superbloque
static inodo inodos[MAX_INODOS];						// Estructura para almacenar los inodos
static unsigned int inodos_uso[MAX_INODOS];				// Estructura auxiliar para controlar si los inodos están abiertos
static unsigned int punteros_lec_esc[MAX_INODOS];		// Estructura auxiliar para almacenar punteros de lectura y escritura
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
	memset(&sbloque.bmapai, '0', sizeof(sbloque.bmapai));
	memset(&sbloque.bmapab, '0', sizeof(sbloque.bmapab));
	/* Se establece el tamaño del dispositivo */
	sbloque.tamDispositivo = deviceSize;
	/* Buffer auxiliar para escribir el superbloque */
	char aux [BLOCK_SIZE];
	memcpy(&aux, &sbloque, BLOCK_SIZE);
	/* Se escribe el superbloque en el disco */
	if(bwrite(DEVICE_IMAGE, 0, aux) == -1)
	{
		/* Se informa si se produce un problema en la escritura en el disco */
		return -1;
	}
	/* Se vacían los metadatos */
	memset(&sbloque, '0', sizeof(sbloque));
	memset(&inodos, '0', sizeof(inodos));

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
	memcpy(&sbloque, &aux, BLOCK_SIZE);
	printf("%d\n", sbloque.numInodos);
	/* Se borra aux */
	memset(&aux, '0', BLOCK_SIZE);
	/* Se lee el bloque que contiene los inodos */
	if(bread(DEVICE_IMAGE, 1, aux) == -1)
	{
		/* Se informa si se produce un problema en la lectura del disco */
		return -1;
	}
	/* Se guardan en memoria los inodos*/
	memcpy(&inodos, &aux, sizeof(inodos));
	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	/* Se comprueba si hay algún inodo en uso */
	for (int i = 0; i < sbloque.numInodos; ++i)
	{
		if (inodos_uso[i] != 0)
		{
			return -1;
		}
	}
	/* Buffer auxiliar para escribir el superbloque */
	char aux [BLOCK_SIZE];
	/* Se copia el superbloque a aux */
	memcpy(&aux, &sbloque, BLOCK_SIZE);
	/* Se escribe el superbloque en el disco */
	if(bwrite(DEVICE_IMAGE, 0, aux) == -1)
	{
		/* Se informa si se produce un problema en la escritura en el disco */
		return -1;
	}
	/* Se borra aux */
	memset(&aux, '0', BLOCK_SIZE);
	/* Se copian los inodos a aux */
	memcpy(&aux, &inodos, sizeof(inodos));
	/* Se escriben los inodos en el disco */
	if(bwrite(DEVICE_IMAGE, 1, aux) == -1)
	{
		/* Se informa si se produce un problema en la escritura en el disco */
		return -1;
	}
	/* Se vacían los metadatos */
	memset(&sbloque, '0', sizeof(sbloque));
	memset(&inodos, '0', sizeof(inodos));

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{
	/* Se comprueba si el nombre ya existe */
	if (namei(fileName) == -1)
	{
		return -1;
	}
	/* Se compruena si hay un inodo libre */
	int inodo = ialloc();
	if (inodo == -1)
	{
		return -2;
	}
	/* Se comprueba si hay un bloque para el indice fichero */
	int bloque = alloc();
	if (bloque == -1)
	{
		return -2;
	}
	/*  */
	/* Si hay un inodo y un bloque */

	return -2;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName)
{
	return -2;
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
	/* Se comprueba que no este en uso */
	if (inodos_uso[inodo] == 0)
	{
		/* Se marca como en uso */
		inodos_uso[inodo] = 1;
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
	if ((fileDescriptor < 0) || (fileDescriptor >= sbloque.numInodos))
	{
		return -1;
	}
	/* Le asignamos el valor 0 para indicar que esta no esta abierto*/
	inodos_uso[fileDescriptor] = 0;
	return 0;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}


/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	/* Se comprueba si el descriptor de fichero corresponde a un fichero */
	if (fileDescriptor < 0 || fileDescriptor >= sbloque.numInodos)
	{
		return -1;
	}
	/* Se comprueba si el offset es menor que 0 */
	if (offset < 0)
	{
		return -1;
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
		punteros_lec_esc[fileDescriptor] = sbloque.inodos[fileDescriptor].tamDispositivo;
		break;

		case FS_SEEK_CUR :
		/* Se establece el puntero de lectura-escritura a offset bytes de su posición actual */
		punteros_lec_esc[fileDescriptor] += offset;
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
	return -2;
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
	for (int i = 0; i < sbloque.numInodos; ++i)
	{
		/* Cuando se encuentra el primero */
		if (bitmap_getbit(sbloque.bmapai, i) == 0)
		{	
			/* Se marca como en uso */
			bitmap_setbit(sbloque.bmapai, i, 1);
			/* Se vacía */
			memset(&inodos[i], '0', sizeof(inodo));
			/* Y se devuelve */
			return i;
		}
	}
	/* Si no se ha encontrado ninguno, se devuelve -1 */
	return -1;
}

/*
 * @brief 	Devuelve el primer bloque libre en el sistema de ficheros
 * @return 	Número del primer bloque libre, -1 si no hay bloques libres.
 */
int alloc(void)
{
	/* Se recorre el mapa de bits de inodos buscando uno libre */
	for (int i = 0; i < sbloque.numBloquesDatos; ++i)
	{
		/* Cuando se encuentra el primero */
		if (bitmap_getbit(sbloque.bmapab, i) == 0)
		{	
			/* Se marca como en uso */
			bitmap_setbit(sbloque.bmapab, i, 1);
			/* Se vacía */
			char aux[BLOCK_SIZE];
			memset(&aux, '0', BLOCK_SIZE);
			bwrite(DEVICE_IMAGE, 1 + i, aux);
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
			/* Se devuelve su número */
			return i;
		}
	}
	/* Si no hay ninguno con ese nomnbre, se devuelve -1 */
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