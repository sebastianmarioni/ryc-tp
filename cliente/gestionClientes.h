#ifndef GESTIONCLIENTES_H_INCLUDED
#define GESTIONCLIENTES_H_INCLUDED
#include "conexion.h"

bool validarCredenciales(string nombreUsuario,string password, string &rol);
string pedirIp();
int pedirPuerto();
void enviarNombreUsuario(SOCKET s);
void enviarContrasena(SOCKET s);
string recibirRol(SOCKET s);
void enviarTraduccion(SOCKET s);
string recibirServerLog(SOCKET s);
string recibirRol(SOCKET s);
void enviarAltaUsuarios(SOCKET s);
void enviarNombreUsuario(SOCKET s);
int enviar(SOCKET socket, string buffer, int tam);
int recibir(SOCKET socket,char *buffer, int tam);

#endif // GESTIONCLIENTES_H_INCLUDED
