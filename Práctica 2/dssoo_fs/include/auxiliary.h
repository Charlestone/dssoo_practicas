/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	01/03/2017
 */

/*
 * @brief 	Devuelve el primer inodo libre en el sistema de ficheros.
 * @return 	Número del primer inodo libre, -1 si no hay inodos libres.
 */
int ialloc(void);

/*
 * @brief 	Devuelve el primer bloque libre en el sistema de ficheros
 * @return 	Número del primer bloque libre, -1 en otro caso.
 */
int alloc(void);

/*
 * @brief 	Comprueba si existe un inodo con cierto nombre
 * @return 	El número del inodo, -1 si no existe un inodo con ese nombre.
 */
int namei(char *fileName);

/*
 * @brief 	Libera un inodo
 * @return 	0 si se libera correctamente, -1 en otro caso.
 */
int ifree(int inodo);

/*
 * @brief 	Libera un bloque
 * @return 	0 si se libera correctamente, -1 en otro caso.
 */
int bfree(int bloque);