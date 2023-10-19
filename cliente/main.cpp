
#include <iostream>
#include <winsock2.h>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>


using namespace std;
bool validarCredenciales(string nombreUsuario,string password, string &rol);
void menuAdminSwitch(SOCKET s);
string pedirIp();
int pedirPuerto();

void enviarNombreUsuario(SOCKET s);
void enviarContrasena(SOCKET s);
bool conectarAServidor(SOCKET clienteSocket, const char* serverIP, int puerto);

string recibirRol(SOCKET s);
SOCKET crearSocketCliente();
SOCKET inicializarCliente(const char* serverIP, int puerto) ;
void cerrarSocket(SOCKET socket);


void enviarTraduccion(SOCKET s);
string recibirServerLog(SOCKET s);

string recibirRol(SOCKET s);
void enviarAltaUsuarios(SOCKET s);
void menuAdminSwitch(SOCKET s);
void enviarNombreUsuario(SOCKET s);
void mostrarOpciones(SOCKET s, string rol);
void menuConsultaSwitch(SOCKET s);
void menuAdmin(SOCKET s);
void menuConsulta(SOCKET s);

int enviar(SOCKET socket, string buffer, int tam);
int recibir(SOCKET socket,char *buffer, int tam);


int main()
{

    //string ip=pedirIp();
    //int puerto=pedirPuerto();
    //SOCKET clienteSocket = inicializarCliente(ip.c_str(), puerto); // Cambia la dirección IP y el puerto del servidor.

    string ip="127.0.0.1";
    int puerto=5005;
    SOCKET clienteSocket = inicializarCliente(ip.c_str(), puerto); // Cambia la dirección IP y el puerto del servidor.

    if (clienteSocket == INVALID_SOCKET) {
                return 1;
    }
    int intentos=3;
    int valido=-1;
    int bloqueado=0;
    bool sesionCerrada=false;

    while(bloqueado!=1 && intentos>0 && !sesionCerrada){

            enviarNombreUsuario(clienteSocket);
            enviarContrasena(clienteSocket);
            recv(clienteSocket,(char*)&bloqueado,sizeof(bloqueado),0);

            if (bloqueado==1){

                char mensajeBloqueo[50];
                memset(mensajeBloqueo,0,sizeof(mensajeBloqueo));
                recv(clienteSocket,mensajeBloqueo,sizeof(mensajeBloqueo),0);
                cout << mensajeBloqueo << endl;
                closesocket(clienteSocket);

            }
            else{

                recv(clienteSocket,(char*)&valido,sizeof(valido),0);
                if (valido==1){
                    string rol= recibirRol(clienteSocket);
                    mostrarOpciones(clienteSocket,rol);
                    sesionCerrada=true;
                }else{
                    intentos --;
                    cout << "Datos de usuario incorrectos. Quedan " << intentos << " intento/s" << endl;
                }

                if (intentos==0){

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
    getline(cin, password);
    send(s,password.c_str(),password.size(),0);


}


SOCKET crearSocketCliente() {
    SOCKET clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket del cliente." << std::endl;
    }
    return clienteSocket;
}

bool conectarAServidor(SOCKET clienteSocket, const char* serverIP, int puerto) {

    bool conectado=true;
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(puerto);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    if (connect(clienteSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al conectar al servidor." << std::endl;
        conectado= false;
    }

    return conectado;
}

SOCKET inicializarCliente(const char* serverIP, int puerto) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock." << std::endl;
        return INVALID_SOCKET;
    }

    SOCKET clienteSocket = crearSocketCliente();
    if (clienteSocket == INVALID_SOCKET) {
        WSACleanup();
        return INVALID_SOCKET;
    }

    if (!conectarAServidor(clienteSocket, serverIP, puerto)) {
        closesocket(clienteSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return clienteSocket;
}

void cerrarSocket(SOCKET socket) {
    closesocket(socket);
    WSACleanup();
}

string recibirRol(SOCKET s){
    char rol[10];
    memset(rol,0,sizeof(rol));
    int bytes_rol= recv(s,rol,sizeof(rol),0);
    return string (rol,bytes_rol);
}

void mostrarOpciones(SOCKET s, string rol){

    if (rol=="ADMIN"){
       menuAdmin(s);
    }

    if (rol=="CONSULTA"){
        menuConsulta(s);
    }
}


void enviarTraduccion(SOCKET s){
    string traduccion;
    cout << "Ingrese la palabra a traducir: " << endl;
    cin >> traduccion;
    send(s,traduccion.c_str(),traduccion.size(),0);

}

string recibirServerLog(SOCKET s){
    char tam[10];
    recv(s,tam,sizeof(tam),0);

    char serverLog[stoi(tam)];
    int recibido= recv(s,serverLog,sizeof(serverLog),0);

    return string(serverLog,recibido);
}


void menuConsulta(SOCKET s){
    int opcion;
    do{
        cout << "1- Traducir |2- Cerrar sesion" << endl;

        cout << "Ingresar opcion: " << endl;
        cin >> opcion;
        send(s,(const char*)&opcion,sizeof(opcion),0);

        if (opcion==1){
            enviarTraduccion(s);
            char traduccion[100];
            memset(traduccion,0,sizeof(traduccion));
            recv(s,traduccion,sizeof(traduccion),0);
            cout << traduccion << endl;
        }
        if (opcion==2){
            cout << "La sesion ha finalizado." << endl;
            cout << "Cerrando sesion..." << endl;
            closesocket(s);
        }
    }while(opcion!=2);

}


void menuAdmin(SOCKET s){
 int opcion;
 int opcionMenuUsuarios;
 string opcionstr;
    do {
    cout << "1-Nueva traduccion|2-Usuarios|-3 Ver Registro|4- Cerrar sesion" << endl;
    cout << "Ingresar opcion: " << endl;
    cin >> opcion;
    send(s,(const char*)&opcion,sizeof(opcion),0);

    if (opcion==1){
        enviarTraduccion(s);
        char mensajeDelServidor[100];
        memset(mensajeDelServidor,0,sizeof(mensajeDelServidor));
        recv(s,mensajeDelServidor,sizeof(mensajeDelServidor),0);
        cout << mensajeDelServidor << endl;
    }

    if (opcion==2){
        do{
            cout << "1-Alta Usuarios | 2-Desbloqueo" << endl;
             // cout << "1-Alta Usuarios | 2-Desbloqueo" << endl;
                cout << "Ingrese /salir para volver al menu principal " << endl;

                cout << "Ingresar opcion: " << endl;
                cin >> opcionstr;

                if (opcionstr=="/salir"){
                    opcionMenuUsuarios=0;
                }
                else{
                     opcionMenuUsuarios= stoi(opcionstr);
                }

          //  int opcionMenuUsuarios;
          //  cout << "Ingresar opcion: " << endl;
          //  cin >> opcionMenuUsuarios;
            send(s,(const char*)&opcionMenuUsuarios,sizeof(opcionMenuUsuarios),0);

            //ALTA
            if (opcionMenuUsuarios==1){
                cout << "-------ALTA USUARIOS--------" << endl;
                //enviarAltaUsuarios(s);
                enviarNombreUsuario(s);
                enviarContrasena(s);

            }
            //DESBLOQUEO
            if (opcionMenuUsuarios==2){

                char listaBloqueados[100];
                recibir(s,listaBloqueados,sizeof(listaBloqueados));
                cout << "Lista de bloqueados: \n" << listaBloqueados << endl;

                if (string(listaBloqueados)!="No se encontraron usuarios bloqueados"){

                    enviarNombreUsuario(s);
                    char mensajeServidor[100];
                    recibir(s,mensajeServidor,sizeof(mensajeServidor));
                    cout << mensajeServidor << endl;

                }

            }

            if (opcion<0 || opcion>2){
                cout <<"Ha ingresado un valor erroneo. Vuelva a intentarlo"<< endl;
            }

        } while(opcionMenuUsuarios!=0);

    }

    if (opcion==3){
        cout << "---------REGISTRO DE ACTIVIDADES--------" << endl;
        string serverLog = recibirServerLog(s);
        cout << serverLog << endl;

    }

    if (opcion==4){
        cout << "La sesion ha finalizado." << endl;
        cout << "Cerrando sesion..." << endl;
        closesocket(s);
    }

    if (opcion<1 || opcion>4){
        cout <<"Ha ingresado un valor erroneo. Vuelva a intentarlo"<< endl;
    }
    }while (opcion!=4);
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


/*
void menuAdminSwitch(SOCKET s){
    int opcion;
    string palabra;
    int opcionMenuUsuarios;
    string opcionstr;

    do{
        cout << "1-Nueva traduccion|2-Usuarios|-3 Ver Registro|4- Cerrar sesion" << endl;
        cout << "Ingresar opcion: " << endl;
        cin >> opcion;

        switch(opcion){
            case 1:
                cout << "NUEVA TRADUCCION" << endl;
                enviarTraduccion(s);
                break;

            case 2:
                do{
                cout << "1-Alta Usuarios | 2-Desbloqueo" << endl;
                cout << "Ingrese /salir para volver al menu principal " << endl;

                cout << "Ingresar opcion: " << endl;
                cin >> opcionstr;

                if (opcionstr=="/salir"){
                    opcionMenuUsuarios=0;
                }
                else{
                     opcionMenuUsuarios= stoi(opcionstr);
                }
                send(s,(const char*)&opcionMenuUsuarios,sizeof(opcionMenuUsuarios),0);
                switch(opcionMenuUsuarios){
                    case 1:
                        cout << "-------ALTA USUARIOS--------" << endl;
                        enviarAltaUsuarios(s);
                        break;

                    case 2:
                        {
                        cout << "-------DESBLOQUEO--------" << endl;
                        char bloqueados[256];
                        int bytes_recibidos = recv(s,bloqueados,sizeof(bloqueados),0);
                        cout << bloqueados << endl;
                        string bloqueadosStr(bloqueados,bytes_recibidos);
                        if (!bloqueadosStr.empty()){
                        cout << "Ingrese un usuario para desbloquearlo: " << endl;
                        }

                        break;

                        }

                    case 0:
                        cout <<"Saliendo del menu de usuarios"<< endl;
                        break;

                    default:
                        cout <<"Ha ingresado un valor incorrecto. Vuelva a intentarlo"<<  endl;
                        break;
                    }

                }while(opcionMenuUsuarios!=0);

            case 3:
                cout << "Registro de actividades: " << endl;
                break;


            case 0:
                cout << "La sesion ha finalizado." << endl;
                cout << "Cerrando sesion..." << endl;
                closesocket(s);
                break;

            default:
                cout << "Ha ingresado un valor incorrecto. Vuelva a intentarlo" << endl;
                break;

        }
    }while (opcion!=0);
}

*/

