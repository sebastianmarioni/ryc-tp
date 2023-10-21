#include "gestionClientes.h"
   #include <iostream>
string pedirIp(){
    string ip;
    cout <<"Ingresar direccion IP: " << endl;
    getline(cin, ip);
    return ip;
}

int pedirPuerto(){
    int puerto;
    cout << "Ingresar puerto: " << endl;
    cin >> puerto;
    return puerto;

}


void enviarNombreUsuario(SOCKET s){

    string nombreUsuario;
    cout << "Ingresar nombre de usuario: " << endl;
    cin >> nombreUsuario;
    send(s,nombreUsuario.c_str(),nombreUsuario.size(),0);

}


void enviarContrasena(SOCKET s){

    string password;
    cout << "Ingresar password: " << endl;
    cin.ignore();
   // getline(cin, password,'\n');
    getline(cin, password);

    send(s,password.c_str(),password.size(),0);
}

string recibirRol(SOCKET s){
    char rol[10];
    memset(rol,0,sizeof(rol));
    int bytes_rol= recv(s,rol,sizeof(rol),0);
    return string (rol,bytes_rol);
}

void enviarTraduccion(SOCKET s){
    string traduccion;
    cout << "Ingrese la palabra a traducir: " << endl;
    cin >> traduccion;
    send(s,traduccion.c_str(),traduccion.size(),0);

}

string recibirServerLog(SOCKET s){
    char tam[10];
    memset(tam,0,sizeof(tam));
    recv(s,tam,sizeof(tam),0);
    cout << " tamanio " << tam <<endl;

    char serverLog[stoi(tam)];
    int recibido= recv(s,serverLog,sizeof(serverLog),0);
    return string(serverLog,recibido);
}


int recibir(SOCKET socket,char *buffer, int tam){

    memset(buffer,0,tam);
    int bytesRecibidos = recv(socket,buffer,tam,0);
    if (bytesRecibidos == SOCKET_ERROR) {
        cerr << "Error al recibir datos: " << WSAGetLastError() << endl;
    }
    return bytesRecibidos;
}


int enviar(SOCKET socket, string buffer, int tam) {
    int bytesEnviados = send(socket, buffer.c_str(), tam,0);

    if (bytesEnviados == SOCKET_ERROR) {
        cerr << "Error al enviar datos: " << WSAGetLastError() << endl;
    }
    return bytesEnviados;
}
