#ifndef GESTIONCLIENTES_H_INCLUDED
#define GESTIONCLIENTES_H_INCLUDED

#include "conexion.h"
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <algorithm>


   // int desbloquear(string nombreUsuario,vector<string>bloqueados);
int desbloquear(std::vector<std::string>& usuarios, const std::string& nombreUsuario);
    void reestablecerIntentos(string nombreUsuario);

    void recibirCredenciales(SOCKET s,string &nombreUsuario, string &password);

    int validarCredenciales(string nombreUsuario,string password, string &rol) ;

    void enviarRegistroDeActividades(SOCKET s);

    string devuelveTraduccion(string palabra);

    bool validarFormato(string palabra,char caracter);

    void serverLog(string mensaje);

    string pedirTraduccion(SOCKET s);

    bool ingresarPalabraEnArchivo(SOCKET s,string palabra);

    bool chequearDatos(string nombreUsuario,string password);

    string obtenerFechaYHoraActual();

    void pasarAMinusculas(string &palabra);

    bool traduccionExistente(string palabra);

    void ingresarUsuarioEnArchivo(SOCKET s, string nombreUsuario,string password);

    bool existeNombreUsuario(string nombreUsuario);

    int usuarioBloqueado(vector<string> bloqueados,string nombreUsuario);

    string devuelveBloqueados(vector<string> bloqueados);

    int recibir(SOCKET socket,char *buffer, int tam);

    int enviar(SOCKET socket, string buffer);

    void aumentarIntentoTxt(string nombreUsuario, string password);

    void reestablecerIntentos(string nombreUsuario);
    void reiniciarIntentos(string usuario);

    int obtenerIntentos(string nombreUsuario);

    void enviaTraduccion(SOCKET s,string palabra);

    string leerServerLog();

    void aumentarIntentos( string nombre,  string contrasena) ;


#endif // GESTIONCLIENTES_H_INCLUDED
