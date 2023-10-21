#include "menu.h"

int main()
{

   // string ip=pedirIp();
   // int puerto=pedirPuerto();
   // SOCKET clienteSocket = inicializarCliente(ip.c_str(), puerto); // Cambia la dirección IP y el puerto del servidor.

    string ip="127.0.0.1";
    int puerto=5005;
    SOCKET clienteSocket = inicializarCliente(ip.c_str(), puerto); // Cambia la dirección IP y el puerto del servidor.

    if (clienteSocket == INVALID_SOCKET) {
        return 1;
    }
    int intentos=0;
    int valido=1;
    bool sesionCerrada=false;

    while(intentos<3 && !sesionCerrada){

        enviarNombreUsuario(clienteSocket);
        enviarContrasena(clienteSocket);
        recibir(clienteSocket,(char*)&intentos,sizeof(intentos));

        if (intentos==3){
            char buffer[50];
            recibir(clienteSocket,buffer,sizeof(buffer));
            cout << buffer << endl;
            closesocket(clienteSocket);

            }
        else{
            recv(clienteSocket,(char*)&valido,sizeof(valido),0);
            if (valido==1){
                string rol= recibirRol(clienteSocket);
                mostrarOpciones(clienteSocket,rol);
                sesionCerrada=true;

            }else{
                intentos++;
                cout << "Datos de usuario incorrectos." << endl;
                }

                if (intentos==3){

                    //buffer dinamicop
                    char mensajeBloqueo[100];
                    int bytes=recv(clienteSocket,mensajeBloqueo,sizeof(mensajeBloqueo),0);
                    cout << string(mensajeBloqueo,bytes) << endl;
                    closesocket(clienteSocket);

                }
            }
    }

    return 0;
}
