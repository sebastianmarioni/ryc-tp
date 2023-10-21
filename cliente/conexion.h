#ifndef CONEXION_H_INCLUDED
#define CONEXION_H_INCLUDED

#include <winsock2.h>
#include <string>
#include <iostream>
using namespace std;

SOCKET crearSocketCliente();
SOCKET inicializarCliente(const char* serverIP, int puerto) ;
void cerrarSocket(SOCKET socket);
bool conectarAServidor(SOCKET clienteSocket, const char* serverIP, int puerto);


#endif // CONEXION_H_INCLUDED
