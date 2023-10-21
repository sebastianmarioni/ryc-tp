#ifndef CONEXION_H_INCLUDED
#define CONEXION_H_INCLUDED

#include <iostream>
#include <winsock2.h>

using namespace std;

SOCKET crearSocketServidor();
void vincularYEscuchar(SOCKET servidorSocket, int puerto) ;
SOCKET aceptarConexion(SOCKET servidorSocket);

#endif // CONEXION_H_INCLUDED
