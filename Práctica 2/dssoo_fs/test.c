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
	/* Se comprueba que no se pueden montar sistemas de ficheros con tamaños fuera del rango */
	ret = mkFS(24*BLOCK_SIZE);
	if(ret == 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se pueden crear sistemas de ficheros de menos de 25 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se pueden crear sistemas de ficheros de menos de 25 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	ret = mkFS(5121*BLOCK_SIZE);
	if(ret == 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se pueden crear sistemas de ficheros de más de 5120 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se pueden crear sistemas de ficheros de más de 5120 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se pueden montar sistemas de ficheros con tamaños dentro del rango */
	ret = mkFS(25*BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se pueden crear sistemas de ficheros de 25 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se pueden crear sistemas de ficheros de 25 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	ret = mkFS(1560*BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se pueden crear sistemas de ficheros de 1560 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se pueden crear sistemas de ficheros de 1560 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	ret = mkFS(DEV_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se pueden crear sistemas de ficheros de 5120 bloques ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se pueden crear sistemas de ficheros de 5120 bloques ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede montar el sistema de ficheros */
	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede montar sistemas de ficheros ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede montar sistemas de ficheros ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede desmontar el sistema de ficheros */
	ret = unmountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede desmontar sistemas de ficheros ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede desmontar sistemas de ficheros ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que se puede crear un fichero */
	ret = mountFS();
	ret = createFile("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede crear un fichero ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede crear un fichero ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se pueden crear dos ficheros con el mismo nombre */
	ret = createFile("test.txt");
	if(ret == 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se pueden crear dos ficheros con el mismo nombre ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se pueden crear dos ficheros con el mismo nombre ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	uint8_t lleno [131072];
	lleno[0] = 1;
	lleno[66523] = 1; 
	lleno [131071] = 1;
	/* Se comprueba que no se puede escribir en ficheros sin abrir */
	escritos = writeFile(0, lleno, sizeof(lleno);
	if (escritos != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se puede escribir en un fichero sin abrir ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se puede escribir en un fichero sin abrir ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede leer en ficheros sin abrir */
	char aux [BLOCK_SIZE];
	leidos = readFile(0, aux, sizeof(aux));
	if (leidos != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se puede leer en un fichero sin abrir ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "No se puede leer en un fichero sin abrir ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba que no se puede leer de un descriptor inválido */
	/* Se comprueba si se puede escribir en un fichero y si este se puede ampliar hasta 1 MiB */
	if (escritos == 4096)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede escribir en un archivo hasta ampliarlo a 1 MiB ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede escribir en un archivo hasta ampliarlo a 1 MiB ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	}
	/* Se comprueba si se puede leer un fichero entero en varias llamadas 
	uint8_t aux [131072];
	leidos = readFile(fd, aux, sizeof(aux)/2);
	leidos += readFile(fd, aux + sizeof(aux)/2, sizeof(aux)/2);
	if (escritos == leidos)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede leer un fichero entero con varias llamadas a read ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "Se puede leer un fichero entero con varias llamadas a read ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
	}
	 Se comprueba que la lectura lee lo que se escribio en la escritura anterior 
	for (int i = 0; i < 131072; ++i)
	{
		if (lleno[i] != aux[i])
		{
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "La lectura y la escritura sin desmontaje y montaje concuerdan ", ANSI_COLOR_RED, "FALSO\n", ANSI_COLOR_RESET);
			break;
		}
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "La lectura y la escritura sin desmontaje y montaje concuerdan ", ANSI_COLOR_GREEN, "VERDADERO\n", ANSI_COLOR_RESET);
	*/


	return 0;
}
