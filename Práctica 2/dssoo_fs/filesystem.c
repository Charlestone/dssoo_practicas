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

static superbloque sbloque;
static inodo inodos[40];

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{	/* Se comprueba si el tamaño deseado excede los límites */
	if (deviceSize < MIN_FS_SIZE || deviceSize > MAX_FS_SIZE){
		return -1;
	}
	/* Se comprueba el número de bloques que tendrá el dispositivo en función de su tamaño */
	if (deviceSize%BLOCK_SIZE != 0){
		sbloque.numBloquesDatos = deviceSize/BLOCK_SIZE + 1;
	} else {
		sbloque.numBloquesDatos = deviceSize/BLOCK_SIZE;
	}
	/* Se ponen a 0 los mapas de bits */
	memset(sbloque.bmapai, '0', sizeof(sbloque.bmapai));
	memset(sbloque.bmapab, '0', sizeof(sbloque.bmapab));
	/* Se establece el tamaño del dispositivo */
	sbloque.tamDispositivo = deviceSize;
	/* Buffer auxiliar para escribir el superbloque */
	char aux [BLOCK_SIZE];
	memcpy(&aux, &sbloque, BLOCK_SIZE);
	/* Se escribe el superbloque en el disco */
	if(bwrite(DEVICE_IMAGE, 0, aux) == -1){
		return -1;
	}
	return 0;

}

/*
 * @brief 	Mounts a file system in the simulated device.0
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{	/* Buffer auxiliar para leer el superbloque */
	char aux [BLOCK_SIZE];
	/* Se lee el superbloque */
	if(bread(DEVICE_IMAGE, 0, aux) == -1){
		return -1;
	}
	/* Se guarda en memoria */
	memcpy(&sbloque, &aux, BLOCK_SIZE);
	/* Se borra aux */
	memset(aux, '0', BLOCK_SIZE);
	/* Se lee el bloque que contiene los inodos */
	if(bread(DEVICE_IMAGE, 1, aux) == -1){
		return -1;
	}
	/* Se guardan en memoria */
	memcpy(&inodos, &aux, 1520);
	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	return -1;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{
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
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	return -1;
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
	return -1;
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
