/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	test.c
 * @brief 	Implementation of the client test routines.
 * @date	01/03/2017
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "include/filesystem.h"
#include "include/metadata.h"


// Color definitions for asserts
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE   "\x1b[34m"

#define N_BLOCKS	5120					// Number of blocks in the device
#define DEV_SIZE 	N_BLOCKS * BLOCK_SIZE	// Device size, in bytes


int main() {
	int ret = 0;
	int fd = 0;
	int leidos = 0;
	int escritos = 0;
	char aux [BLOCK_SIZE];
	uint8_t lleno [131072];
	lleno[0] = 1;
	lleno[66523] = 1; 
	lleno [131071] = 1;
	/* Se comprueba que no se pueden montar sistemas de ficheros con tamaños fuera del rango */
	/* CP 1 */
	ret = mkFS(24*BLOCK_SIZE);
	if(ret == 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 1 No se pueden crear sistemas de ficheros de menos de 24 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 1 No se pueden crear sistemas de ficheros de menos de 24 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* CP 2 */
	ret = mkFS(5121*BLOCK_SIZE);
	if(ret == 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 2 No se pueden crear sistemas de ficheros de más de 5121 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 2 No se pueden crear sistemas de ficheros de más de 5121 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se pueden montar sistemas de ficheros con tamaños dentro del rango */
	/* CP 3 */
	ret = mkFS(25*BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 3 Se pueden crear sistemas de ficheros de 25 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 3 Se pueden crear sistemas de ficheros de 25 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* CP 4 */
	ret = mkFS(1560*BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 4 Se pueden crear sistemas de ficheros de 1560 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 4 Se pueden crear sistemas de ficheros de 1560 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* CP 5 */
	ret = mkFS(DEV_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 5 Se pueden crear sistemas de ficheros de 5120 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 5 Se pueden crear sistemas de ficheros de 5120 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede montar el sistema de ficheros */
	/* CP 6 */
	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 6 Se puede montar sistemas de ficheros ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 6 Se puede montar sistemas de ficheros ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede desmontar el sistema de ficheros */
	/* CP 7 */
	ret = unmountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 7 Se puede desmontar sistemas de ficheros ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 7 Se puede desmontar sistemas de ficheros ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede crear un fichero */
	/* CP 8 */
	ret = mountFS();
	ret = createFile("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 8 Se puede crear un fichero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 8 Se puede crear un fichero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se pueden crear dos ficheros con el mismo nombre */
	/* CP 9 */
	ret = createFile("test.txt");
	if(ret != -1){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 9 No se pueden crear dos ficheros con el mismo nombre ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 9 No se pueden crear dos ficheros con el mismo nombre ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se pueden crear más de 40 ficheros */
	/* CP 10 */
	for (int i = 0; i < 40; ++i)
	{
		char a = i;
		ret = createFile(&a);
		if (ret != 0 && i != 39)
		{
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 10 No se pueden crear más de 40 ficheros ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
			break;
		}
		if (ret == -2 && i == 39)
		{
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 10 No se pueden crear más de 40 ficheros ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
		}
	}
	/* Se comprueba que no se puede escribir en ficheros sin abrir */
	/* CP 11 */
	escritos = writeFile(0, lleno, sizeof(lleno));
	if (escritos != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 11 No se puede escribir en un fichero sin abrir ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 11 No se puede escribir en un fichero sin abrir ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede leer en ficheros sin abrir */
	/* CP 12 */
	
	leidos = readFile(0, aux, sizeof(aux));
	if (leidos != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 12 No se puede leer en un fichero sin abrir ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 12 No se puede leer en un fichero sin abrir ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede abrir un fichero que no se ha creado */
	/* CP 13 */
	fd = openFile("noexisto.txt");
	if (fd != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 13 No se puede abrir un fichero que no se ha creado ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 13 No se puede abrir un fichero que no se ha creado ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede abrir un fichero que existe */
	/* CP 14 */
	fd = openFile("test.txt");
	if (fd != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 14 Se puede abrir un fichero que existe ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 14 Se puede abrir un fichero que existe ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede abrir dos veces un fichero */
	/* CP 15 */
	ret = openFile("test.txt");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 15 No se puede abrir dos veces un fichero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 15 No se puede abrir dos veces un fichero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede escribir en un descriptor inválido */
	/* CP 16 */
	escritos = writeFile(-1, lleno, sizeof(lleno));
	if (escritos != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 16 No se puede escribir en descriptor inválido ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 16 No se puede escribir en descriptor inválido ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede leer de un descriptor inválido */
	/* CP 17 */
	leidos = readFile(-1, aux, sizeof(aux));
	if (leidos != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 17 No se puede leer de un descriptor inválido ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 17 No se puede leer de un descriptor inválido ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede cerrar un fichero abierto */
	/* CP 18 */
	ret = closeFile(fd);
	if (fd != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 18 Se puede cerrar un fichero abierto ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 18 Se puede cerrar un fichero abierto ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede cerrar un archivo ya cerrado */
	/* CP 19 */
	ret = closeFile(fd);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 19 No se puede cerrar un archivo ya cerrado ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 19 No se puede cerrar un archivo ya cerrado ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede cerrar un archivo con descriptor de fichero inválido */
	/* CP 20 */
	ret = closeFile(-1);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 20 No se puede cerrar un decsriptor inválido ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 20 No se puede cerrar un decsriptor inválido ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba si se puede escribir en un fichero */
	/* CP 21 */
	fd = openFile("test.txt");
	escritos = writeFile(fd, lleno, 1);
	if (escritos != 1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 21 Se puede escribir en un fichero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 21 Se puede escribir en un fichero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que la función lseek admite whence 2 */
	/* CP 22 */
	ret = lseekFile(fd, 20, 2);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 22 La función lseek admite whence 2 ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 22 La función lseek admite whence 2 ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que la función lseek admite numBytes positivo y que la función lseek admite whence 0*/
	/* CP 23 */
	ret = lseekFile(fd, 1, 0);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 23 La función lseek admite numBytes positivo y whence 0 ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 23 La función lseek admite numBytes positivo y whence 0 ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que la función lseek admite numBytes negativo */
	/* CP 24 */
	ret = lseekFile(fd, -1, 0);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 24 La función lseek admite numBytes negativo ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 24 La función lseek admite numBytes negativo ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que la función lseek no admite numBytes que sobrepasen los límtes del fichero desde la posición actual del puntero */
	/* CP 25 */
	ret = lseekFile(fd, 4000,  0);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 25 La función lseek no admite numBytes que sobrepasen los límtes del fichero desde la posición actual del puntero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 25 La función lseek no admite numBytes que sobrepasen los límtes del fichero desde la posición actual del puntero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que la función lseek admite whence 1 */
	/* CP 26 */
	ret = lseekFile(fd, 20, 1);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 26 La función lseek admite whence 1 ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 26 La función lseek admite whence 1 ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que la función lseek no admite valores de whence distintos de 0, 1, 2 */
	/* CP 27 */
	ret = lseekFile(fd, 20, 6);
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 27 La función lseek no admite valores de whence distintos de 0, 1, 2 ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 27 La función lseek no admite valores de whence distintos de 0, 1, 2 ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede leer de un fichero */
	/* CP 28 */
	ret = lseekFile(fd, 0, 2);
	leidos = readFile(fd, aux, 1);
	if (leidos != 1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 28 Se puede leer de un fichero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 28 Se puede leer de un fichero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que lo leido es igual que lo escrito previamente sobre un mismo fichero */
	/* CP 29 */
	if (lleno[0] != (uint8_t) aux[0])
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 29 Se lee lo mismo que se ha escrito en un fichero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 29 Se lee lo mismo que se ha escrito en un fichero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba si se puede leer un fichero entero en varias llamadas */
	/* CP 30 */
	escritos = writeFile(fd, lleno, 1);
	ret = lseekFile(fd, 0, 2);
	leidos = readFile(fd, aux, 1);
	leidos += readFile(fd, aux, 1);
	if (leidos == 2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 30 Se puede leer un fichero entero con varias llamadas a read ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 30 Se puede leer un fichero entero con varias llamadas a read ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede borrar un archivo si no está cerrado */
	/* CP 31 */
	ret = removeFile("test.txt");
	if (ret != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 31 No se puede borrar un archivo si no está cerrado ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 31 No se puede borrar un archivo si no está cerrado ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede borrar un archivo ya cerrado */
	/* CP 32 */
	ret = closeFile(fd);
	ret = removeFile("test.txt");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 32 Se puede borrar un archivo ya cerrado ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 32 Se puede borrar un archivo ya cerrado ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede borrar un fichero que no existe */
	/* CP 33 */
	ret = removeFile("noexisto.txt");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 33 No se puede borrar un fichero que no existe ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 33 No se puede borrar un fichero que no existe ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se mantienen los metadatos después de un desmontaje */
	/* CP 34 */
	ret = createFile("test.txt");
	ret = unmountFS();
	ret = mountFS();
	ret = createFile("test.txt");
	if (ret != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 34 Se mantienen los metadatos después de un desmontaje ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 34 Se mantienen los metadatos después de un desmontaje ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede escribir en un archivo hasta ampliar su tamaño a 1 MiB*/
	/* CP 35 */
	fd = openFile("test.txt");
	for (int i = 0; i < 131072; ++i)
	{
		memcpy(&aux, &lleno[i], sizeof(uint8_t));
		ret = writeFile(fd, aux, sizeof(uint8_t));
		if (ret == -1)
		{
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 35 Se puede escribir en un archivo hasta ampliar su tamaño a 1 MiB ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
			break;
		}
	}
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "CP 35 Se puede escribir en un archivo hasta ampliar su tamaño a 1 MiB ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	return 0;
}
