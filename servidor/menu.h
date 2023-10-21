#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED


#include "gestionClientes.h"
void menuConsulta(SOCKET s,string usuario);
void menuAdmin(SOCKET s,string usuario,vector<string>bloqueados);
void mostrarOpciones(SOCKET s, string rol,string usuario,vector<string>bloqueados);


#endif // MENU_H_INCLUDED
