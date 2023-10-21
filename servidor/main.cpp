#include "menu.h"

using namespace std;

int main()
{

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock." << std::endl;
        return 1;
    }

    SOCKET servidorSocket = crearSocketServidor();
    int puerto=5005;
    vincularYEscuchar(servidorSocket, puerto); // Puerto del servidor
    serverLog("=======Inicia Servidor=======");
    serverLog("Socket creado. Puerto de escucha: " + to_string(puerto));
    cout << "Esperando conexiones entrantes..." << endl;

    string nombreUsuario;
    string password;
    string rol="";
    vector<string> usuariosBloqueados;
    usuariosBloqueados.push_back("carlos");
    usuariosBloqueados.push_back("roberto");




    while (true) {

        //antes estaban afuera del while
        int valido;
        bool sesionCerrada=false;

        //valor por defecto para que entre al bucle
        int intentos=0;

        SOCKET clienteSocket = aceptarConexion(servidorSocket);

        while (intentos<3 && !sesionCerrada) {

            recibirCredenciales(clienteSocket,nombreUsuario,password);
            intentos=obtenerIntentos(nombreUsuario);
            send(clienteSocket,(char*)&intentos,sizeof(intentos),0);
            if (intentos==3){
                string mensaje="El usuario " +nombreUsuario +" esta bloqueado";
                enviar(clienteSocket,mensaje);
                usuariosBloqueados.push_back(nombreUsuario);
                closesocket(clienteSocket);
            }

            else{
                valido= validarCredenciales(nombreUsuario,password,rol);
                send(clienteSocket,(char*)&valido,sizeof(valido),0);

                if (valido==1){
                    serverLog("Inicio de sesion – usuario: " + nombreUsuario);
                    send(clienteSocket,rol.c_str(),rol.size(),0);
                    mostrarOpciones(clienteSocket,rol,nombreUsuario,usuariosBloqueados);
                    sesionCerrada=true;

                }else{
                    intentos++;
                    aumentarIntentos(nombreUsuario,password);
                }

                if (intentos==3){
                    usuariosBloqueados.push_back(nombreUsuario);
                    string mensaje="Se ha superado la cantidad maxima de intentos, la cuenta " + nombreUsuario + " ha sido bloqueada";
                    enviar(clienteSocket,mensaje);
                    closesocket(clienteSocket);
                }
            }
        }

    }

    closesocket(servidorSocket);
    WSACleanup();
    return 0;
}

