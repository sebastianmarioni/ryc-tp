#include <iostream>
#include <winsock2.h>

#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdio>

#include <ctime>

using namespace std;

string leerServerLog();
void vincularYEscuchar(SOCKET servidorSocket, int puerto);
SOCKET crearSocketServidor();
SOCKET aceptarConexion(SOCKET servidorSocket);
void recibirCredenciales(SOCKET s,string &nombreUsuario, string &password);
int validarCredenciales(string nombreUsuario,string password, string &rol) ;
void menuConsulta(SOCKET s,string usuario);
void enviarRegistroDeActividades(SOCKET s);
string devuelveTraduccion(string palabra);
bool validarFormato(string palabra,char caracter);
void mostrarOpciones(SOCKET s, string rol,string usuario,vector<string>bloqueados);
void serverLog(string mensaje);
void menuAdmin(SOCKET s,string usuario,vector<string>bloqueados);
string pedirTraduccion(SOCKET s);
bool ingresarPalabraEnArchivo(SOCKET s,string palabra);
bool chequearDatos(string nombreUsuario,string password);
string obtenerFechaYHoraActual();
void pasarAMinusculas(string &palabra);
bool traduccionExistente(string palabra);
void enviarListaBloqueados(SOCKET s,vector<string> bloqueados);
void ingresarUsuarioEnArchivo(SOCKET s, string nombreUsuario,string password);
bool existeNombreUsuario(string nombreUsuario);
int usuarioBloqueado(vector<string> bloqueados,string nombreUsuario);
int desbloquear(string nombreUsuario,vector<string>&bloqueados);
string devuelveBloqueados(vector<string> bloqueados);
int recibir(SOCKET socket,char *buffer, int tam);
int enviar(SOCKET socket, string buffer);
void aumentarIntentoTxt(string nombreUsuario, string password);
void reestablecerIntentos(string nombreUsuario);
int obtenerIntentos(string nombreUsuario);

int main()
{

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock." << std::endl;
        return 1;
    }

    SOCKET servidorSocket = crearSocketServidor();

    //Pasar puerto por parametro en el main?
    vincularYEscuchar(servidorSocket, 5005); // Puerto del servidor

    cout << "Esperando conexiones entrantes..." << endl;

    int intentos=3;
    string nombreUsuario;
    string password;
    string rol="";
    vector<string> usuariosBloqueados;
    int valido;
    usuariosBloqueados.push_back("pepe");
    usuariosBloqueados.push_back("maria");


    int bloqueado=0;

    while (true) {
        //Hacerlo con esta logica... Por las dudas.
        //int intentos=obtenerIntentos(nombreUsuario);
        SOCKET clienteSocket = aceptarConexion(servidorSocket);

        while (intentos>0 && bloqueado!=1){

            recibirCredenciales(clienteSocket,nombreUsuario,password);
            bloqueado=usuarioBloqueado(usuariosBloqueados,nombreUsuario);
            send(clienteSocket,(char*)&bloqueado,sizeof(bloqueado),0);

            //Enviar el int
            if (bloqueado==1){
            //Ponerlo del lado del cliente, para eso tengo que cambiar la funcion de enviar nombre asi puedo acceder al nombre desde el cliente
                string mensajeBloqueo= "El usuario " +nombreUsuario +" esta bloqueado";
                send(clienteSocket,mensajeBloqueo.c_str(),mensajeBloqueo.size(),0);

                closesocket(clienteSocket);
                //HAY UN ERROR CUANDO EL USUARIO BLOQUEADO QUIERE VOLVER A INGRESAR, INTENTA ENVIAR ALGO Y FALLA. AVERIGUAR
                //bloqueado=0; CUANDO NO  ESTA ESTO FUNCIONA BIEN... PROBARLO BIEN ANTES DE TOMAR UNA DECISION.
            }
            else{

                valido= validarCredenciales(nombreUsuario,password,rol);
                send(clienteSocket,( char*)&valido,sizeof(valido),0);

                if (valido==1){
                    serverLog("Inicio de sesión – usuario: " + nombreUsuario);
                    send(clienteSocket,rol.c_str(),rol.size(),0);
                    mostrarOpciones(clienteSocket,rol,nombreUsuario,usuariosBloqueados);

                }else{
                    intentos--;
                     aumentarIntentoTxt(nombreUsuario,password);
                }
                if (intentos==0){
                    usuariosBloqueados.push_back(nombreUsuario);
                    string mensaje="Se ha superado la cantidad maxima de intentos, la cuenta " + nombreUsuario + " ha sido bloqueada";
                    enviar(clienteSocket,mensaje);
                    closesocket(clienteSocket);
                }
            }
        }

    intentos=3;

    }
    closesocket(servidorSocket);
    WSACleanup();
    return 0;
}




SOCKET crearSocketServidor() {
    SOCKET servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
    //Tirar excepciones? Quizas un boolean
    if (servidorSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket del servidor." << std::endl;
    }

    return servidorSocket;
}

void vincularYEscuchar(SOCKET servidorSocket, int puerto) {

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    direccionServidor.sin_addr.s_addr = INADDR_ANY;

    //Tirar excepciones? Quizas un boolean
    if (bind(servidorSocket, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        closesocket(servidorSocket);
        cout << "Error al vincular el socket"<<  endl;
    }

    //Tirar excepciones?
    if (listen(servidorSocket, 5) == SOCKET_ERROR) {
        closesocket(servidorSocket);
        cout << "Error al escuchar en el socket"<< endl;
    }

    serverLog("=======Inicia Servidor=======");
    serverLog("Socket creado. Puerto de escucha: " + to_string(puerto));

}

SOCKET aceptarConexion(SOCKET servidorSocket) {

    sockaddr_in direccionCliente;
    int tamanoDireccionCliente = sizeof(direccionCliente);
    SOCKET clienteSocket = accept(servidorSocket, (struct sockaddr*)&direccionCliente, &tamanoDireccionCliente);
     //Tirar excepciones? Quizas un boolean
    if (clienteSocket == INVALID_SOCKET) {
        closesocket(servidorSocket);
    }
    return clienteSocket;
}

void recibirCredenciales(SOCKET s,string &nombreUsuario, string &password){

    char buffer[30];
    memset(buffer,0,sizeof(buffer));

    // recibo el nombre de usuario
    recv(s,buffer,sizeof(buffer),0);
    nombreUsuario=buffer;

    //recibo el password
    memset(buffer,0,sizeof(buffer));
    recv(s,buffer,sizeof(buffer),0);
    password=buffer;

}

int validarCredenciales(string nombreUsuario,string password, string &rol) {
    ifstream archivo("credenciales.txt");
    int credencialesValidas=-1;
    string mensajeAlUsuario="Datos de usuario incorrectos";

    string linea;
    while (getline(archivo, linea) && credencialesValidas==-1){
        istringstream ss(linea);
        string elemento;
        vector<std::string> elementos;

        while (getline(ss, elemento, '|')) {
            elementos.push_back(elemento);
        }

        if (elementos.size() == 4)  {
            if (nombreUsuario==elementos[0] && password==elementos[1]){
                rol=elementos[2];
                credencialesValidas= 1;

            }
        }
    }

    archivo.close();
    return credencialesValidas;
}


//---


//No deberia validar nada, porque llegados a este punto no es posible que lleguen otras opciones distinas a admin o consulta(CREO)
void mostrarOpciones(SOCKET s, string rol,string usuario,vector<string>bloqueados){

    //Fijarse si podes la opcion por parametro de una sola vez en lugar de applicarla en los dos menus.
  if (rol=="ADMIN"){
    menuAdmin(s,usuario,bloqueados);

  }

  if (rol=="CONSULTA"){
    menuConsulta(s,usuario);
  }

}

//Hacerlo con switch case
void menuAdmin(SOCKET s,string usuario,vector<string>bloqueados){

    //recibo la opcion que me manda el cliente
    int opcion;
    int opcionMenuUsuarios;
    do{

    //int opcion;
    recv(s,(char*)&opcion,sizeof(opcion),0);

    if (opcion==1){
        string traduccion=  pedirTraduccion(s);
        cout << traduccion << endl;
        ingresarPalabraEnArchivo(s,traduccion);
    }

    if (opcion==2){
            do{
                recv(s,(char*)&opcionMenuUsuarios,sizeof(opcionMenuUsuarios),0);
                if (opcionMenuUsuarios==1){
                    string usuario;
                    string password;
                    recibirCredenciales(s,usuario,password);
                    ingresarUsuarioEnArchivo(s,usuario,password);
                }
                if (opcionMenuUsuarios==2){

                    string listaBloqueados=devuelveBloqueados(bloqueados);

                    if (listaBloqueados.empty()){
                        string mensaje="No se encontraron usuarios bloqueados";
                        enviar(s,mensaje);

                    }else{
                        cout << enviar(s,listaBloqueados) << endl;
                        char nombreUsuario[30];
                        recibir(s,nombreUsuario,sizeof(nombreUsuario));

                        if (desbloquear(string(nombreUsuario),bloqueados)==1){
                            enviar(s,string(nombreUsuario)+ "desbloqueado correctamente");
                            reestablecerIntentos(nombreUsuario);
                        }
                        else{
                            enviar(s,string(nombreUsuario)+ " no se encuentra en la lista de bloqueados");
                        }
                    }
                }

            }while(opcionMenuUsuarios!=0);


    }
    //Registro de actividades.
    if (opcion==3){
        enviarRegistroDeActividades(s);
        cout << "REGISTRO ENVIADO AL USUARIO"<< endl;

    }
    //Cerrar sesion.
    if (opcion==4){
        closesocket(s);
        //string mensaje="Cierre de sesión – usuario: " + usuario;
        serverLog("Cierre de sesión – usuario: " + usuario);
    }

    }while (opcion!=4);



}

string pedirTraduccion(SOCKET s){

    char recibido[50];
    int cantidad_bytes= recv(s,recibido,sizeof(recibido),0);
    return string(recibido,cantidad_bytes);


}

bool ingresarPalabraEnArchivo(SOCKET s,string palabra){

    int posicionDelDelimitadorTxt = palabra.find(':');
    string palabraEnIngles = palabra.substr(0,posicionDelDelimitadorTxt);
    string palabraEnEspanol =  devuelveTraduccion(palabraEnIngles);
    bool ingresada=false;

    bool formatoValido=validarFormato(palabra,':');
    string mensajeAlCliente;

    if (!formatoValido){
        mensajeAlCliente="El formato de insercion debe ser palabraEnIngles:traduccionEnEspañol";
    }

    bool existePalabra=traduccionExistente(palabra);

    if (existePalabra){
       mensajeAlCliente= "Ya existe una traduccion para " + palabraEnIngles+":"+palabraEnEspanol;
    }

    ofstream archivo("traducciones.txt", ios::app);

    if ((existePalabra==false) && formatoValido && archivo.is_open()){

        archivo << palabra << endl;
        archivo.close();
        ingresada=true;
        mensajeAlCliente= "Traduccion agregada con exito";
    }

    send(s,mensajeAlCliente.c_str(),mensajeAlCliente.size(),0);

    return ingresada;
}

void ingresarUsuarioEnArchivo(SOCKET s, string nombreUsuario,string password){

        pasarAMinusculas(nombreUsuario);
        pasarAMinusculas(password);

        string mensajeAlCliente;
        bool valido=true;

        if (chequearDatos(nombreUsuario,password)){
            valido=false;
            mensajeAlCliente="Error al dar de alta el nuevo usuario: datos incompletos.";
        }

        if (existeNombreUsuario(nombreUsuario)){
            valido=false;
            mensajeAlCliente="Error al dar de alta el nuevo usuario: usuario existente.";
        }

        ofstream archivo("credenciales.txt",ios::app);
        if (!archivo.is_open()) {
            valido=false;
            mensajeAlCliente="Error al abrir el archivo";
        }

        if (valido){
            archivo << "\n"<< nombreUsuario << "|" << password << "|CONSULTA|0";
            mensajeAlCliente = nombreUsuario + " dado de alta correctamente";
        }

        send(s,mensajeAlCliente.c_str(),mensajeAlCliente.size(),0);
        archivo.close();

}

//REVISAR!!
void pasarAMinusculas(string &palabra){

    for (char& c : palabra) {
        c = tolower(c);
    }

}

bool chequearDatos(string nombreUsuario,string password){

    return nombreUsuario.empty() ||  password.empty();

}

bool existeNombreUsuario(string nombreUsuario){
    ifstream archivo("credenciales.txt");
    bool existe=false;

    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo.");
    }

    string linea;
    while (getline(archivo, linea) && existe==false){
        istringstream ss(linea);
        string elemento;
        vector<std::string> elementos;

        while (getline(ss, elemento, '|')) {
            elementos.push_back(elemento);
        }

        if (elementos.size() == 4)  {
            if (nombreUsuario==elementos[0]){
                existe=true;
            }
        }
    }

    archivo.close();

    return existe;
}

bool existeContrasena(string contrasena){
    ifstream archivo("credenciales.txt");
    bool existe=false;

    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo.");
    }

    string linea;
    while (getline(archivo, linea) && existe==false){
        istringstream ss(linea);
        string elemento;
        vector<std::string> elementos;

        while (getline(ss, elemento, '|')) {
            elementos.push_back(elemento);
        }

        if (elementos.size() == 4)  {
            if (contrasena==elementos[1]){
                existe=true;
            }
        }
    }

    archivo.close();

    return existe;
}

void serverLog(string mensaje){

    std::ofstream archivo("serverLog.txt",ios::app);

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;

    }
    string hora= obtenerFechaYHoraActual();
    archivo << "[" << hora << "]" << mensaje << "\n";
    archivo.close();
}

string obtenerFechaYHoraActual() {

    time_t t = std::time(nullptr);
    tm* now = std::localtime(&t);

    char buffer[50];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);

    return string(buffer);
}

//REVISAR ESTO!!
void enviarRegistroDeActividades(SOCKET s){

    string serverLog = leerServerLog();
    string tamanio= to_string(serverLog.size());

    send(s,tamanio.c_str(),tamanio.size(),0);
    send(s,serverLog.c_str(),serverLog.size(),0);
}

string leerServerLog(){

    ifstream archivo("serverLog.txt");
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        throw runtime_error("Error al intentar abrir el archivo.");
    }

    string serverLog;
    string linea;
    while (std::getline(archivo, linea)) {
        serverLog += linea + "\n";
    }

    // Cierra el archivo después de leerlo
    archivo.close();
    return serverLog;

}

bool validarFormato(string palabra,char caracter){
    int contador=0;
    bool valido=true;
    //Primero me fijo si hay ':'. Pero para mas seguridad voy a contar que solo este un sola vez elcaracter.
    //Ademas el caracter ':' no debe estar ni en el inicio ni el fin de la palabra recibida.

    for (unsigned int i=0;i<(palabra.size());i++){
        if (palabra[i]==caracter){
            contador++;
        }
        if (palabra[0]==caracter){
            valido= false;
        }
        if (palabra[palabra.size()-1] == caracter ){
            valido=false;
        }
    }
    if (contador!=1){
        valido=false;

    }

    return valido;
}

bool traduccionExistente(string palabra){
    bool existe = false;

    ifstream archivo("traducciones.txt");

    if (archivo.is_open()) {
        string linea;
        while (getline(archivo, linea)&& !existe) {

            int posicionDelDelimitadorTxt = linea.find(':');
            int posicionDelDelimitadorCliente = palabra.find(':');

            //Ambas serian las palabras en ingles
            string subStringTxt = linea.substr(0,posicionDelDelimitadorTxt);
            string subStringCliente = palabra.substr(0,posicionDelDelimitadorCliente);

            if (subStringCliente == subStringTxt){
                existe= true;
                archivo.close();
            }
        }

        archivo.close();

    } else {
        cerr << "No se pudo abrir el archivo.\n";
    }

    return existe;
}


string devuelveTraduccion(string palabra){
    bool existe = false;
    ifstream archivo("traducciones.txt");
    string traduccion="";
    if (archivo.is_open()) {
        string linea;
        while (getline(archivo, linea)&& !existe) {
            int posicionDelDelimitadorTxt = linea.find(':');
            string palabraInglesTxt = linea.substr(0,posicionDelDelimitadorTxt);
            string palabraEspTxt = linea.substr(posicionDelDelimitadorTxt+1);

            if (palabraInglesTxt==palabra){
                traduccion=palabraEspTxt;
                existe=true;
                //send(s,traduccion.c_str(),traduccion.size(),0);
                archivo.close();
            }
        }
        archivo.close();
    }

    return traduccion;
}

void enviaTraduccion(SOCKET s,string palabra){
    bool existe = false;
    string mensajeAlCliente;
    ifstream archivo("traducciones.txt");
    string traduccion="";
    string palabraInglesTxt;
    string palabraEspTxt;
    if (archivo.is_open()) {
        string linea;
        while (getline(archivo, linea)&& !existe) {
            int posicionDelDelimitadorTxt = linea.find(':');
            palabraInglesTxt = linea.substr(0,posicionDelDelimitadorTxt);
            palabraEspTxt = linea.substr(posicionDelDelimitadorTxt+1);

            if (palabraInglesTxt==palabra){
                traduccion=palabraEspTxt;
                mensajeAlCliente= palabraInglesTxt + " en ingles es " + palabraEspTxt + " en espanol";
                existe=true;
                archivo.close();
            }
        }

        if (!existe){
            mensajeAlCliente="No fue posible encontrar la traduccion para :" + palabra;
        }

        send(s,mensajeAlCliente.c_str(),mensajeAlCliente.size(),0);
        archivo.close();
    }

}

void menuConsulta(SOCKET s,string usuario){

    int opcion;
    do{
        recv(s,(char*)&opcion,sizeof(opcion),0);

        if (opcion==1){
            string traduccionEsp=  pedirTraduccion(s);
            pasarAMinusculas(traduccionEsp);
            enviaTraduccion(s,traduccionEsp);

        }
        if (opcion==2){
            closesocket(s);
            string mensaje="Cierre de sesión – usuario: " + usuario;
            serverLog(mensaje);
        }
    }while (opcion!=2);

}


string devuelveBloqueados(vector<string> bloqueados){

    string listaBloqueados;

    for (string usuario:bloqueados){
        listaBloqueados.append(usuario+"\n");
    }

    return listaBloqueados;
}



/*
void enviarListaBloqueados(SOCKET s,vector<string> bloqueados){

    string listaBloqueados;

    for (string usuario:bloqueados){
        listaBloqueados.append(usuario+"\n");
    }

   // if (listaBloqueados.empty()){
   //     listaBloqueados="No se encontraron usuarios bloqueados";
   // }
    int bytes_recibidos=send(s,listaBloqueados.c_str(),listaBloqueados.size(),0);
    cout << "Bytes lista bloqueados:" << bytes_recibidos << endl;

}
*/
///Retorna 1(true) si esta bloqueado. Retorna 0 si no esta bloqueado
int usuarioBloqueado(vector<string> bloqueados,string nombreUsuario){
    int bloqueado= 0;

    for (string usuario: bloqueados){
        if (nombreUsuario==usuario){
            bloqueado=1;
        }
    }
    return bloqueado;
}

//1 si lo desbloqueo
//0 si no lo desbloqueo(porque no lo encontro)

int desbloquear(string nombreUsuario,vector<string>&bloqueados){
    int encontrado=0;
    int posicion=0;

    for (unsigned i = 0; i < bloqueados.size(); i++) {
        if (nombreUsuario == bloqueados[i]) {
             posicion=i; // Usuario encontrado en la lista de bloqueados
             encontrado=1;
             bloqueados.erase(bloqueados.begin()+posicion);
        }
    }
   // if (encontrado){
    //    bloqueados.erase(bloqueados.begin()+posicion);
   // }
    return encontrado;
}

int recibir(SOCKET socket,char *buffer, int tam){

    memset(buffer,0,tam);
    int bytesRecibidos = recv(socket,buffer,tam,0);
    if (bytesRecibidos == SOCKET_ERROR) {
        cerr << "Error al recibir datos: " << WSAGetLastError() << endl;
    }
    return bytesRecibidos;
}

int enviar(SOCKET socket, string buffer) {

    int bytesEnviados = send(socket, buffer.c_str(), buffer.size(),0);
    if (bytesEnviados == SOCKET_ERROR) {
        cerr << "Error al enviar datos: " << WSAGetLastError() << endl;
    }
    return bytesEnviados;
}

void aumentarIntentoTxt(string nombreUsuario,string password) {
    // Leer el contenido completo del archivo en memoria
    std::ifstream archivoEntrada("credenciales.txt");
    std::string contenido;
    std::string linea;

    if (!archivoEntrada.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return;
    }

    while (std::getline(archivoEntrada, linea)) {
        contenido += linea + "\n";
    }

    archivoEntrada.close();

    // Buscar y actualizar la cantidad de intentos en memoria
    std::string nuevoContenido;
    std::istringstream contenidoStream(contenido);
    bool usuarioEncontrado = false;

    while (std::getline(contenidoStream, linea)) {
        std::istringstream ss(linea);
        std::string elemento;
        std::vector<std::string> elementos;

        while (std::getline(ss, elemento, '|')) {
            elementos.push_back(elemento);
        }

        if (elementos.size() == 4 && nombreUsuario == elementos[0] && password != elementos[1]) {
            int intentos = std::stoi(elementos[3]) + 1;
            elementos[3] = std::to_string(intentos);
            usuarioEncontrado = true;
        }

        nuevoContenido += elementos[0] + "|" + elementos[1] + "|" + elementos[2] + "|" + elementos[3] + "\n";
    }

    // Sobrescribir el archivo completo con el contenido actualizado
    std::ofstream archivoSalida("credenciales.txt");

    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura." << std::endl;
        return;
    }

    archivoSalida << nuevoContenido;
    archivoSalida.close();

    if (usuarioEncontrado) {
        std::cout << "Cantidad de intentos actualizada con éxito." << std::endl;
    } else {
        std::cerr << "Usuario no encontrado en el archivo." << std::endl;
    }
}


void reestablecerIntentos(string nombreUsuario) {
    // Leer el contenido completo del archivo en memoria
    std::ifstream archivoEntrada("credenciales.txt");
    std::string contenido;
    std::string linea;

    if (!archivoEntrada.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return;
    }

    while (std::getline(archivoEntrada, linea)) {
        contenido += linea + "\n";
    }

    archivoEntrada.close();

    // Buscar y reiniciar la cantidad de intentos en memoria
    std::string nuevoContenido;
    std::istringstream contenidoStream(contenido);
    bool usuarioEncontrado = false;

    while (std::getline(contenidoStream, linea)) {
        std::istringstream ss(linea);
        std::string elemento;
        std::vector<std::string> elementos;

        while (std::getline(ss, elemento, '|')) {
            elementos.push_back(elemento);
        }

        if (elementos.size() == 4 && nombreUsuario == elementos[0]) {
            elementos[3] = "0"; // Establecer la cantidad de intentos en 0
            usuarioEncontrado = true;
        }

        nuevoContenido += elementos[0] + "|" + elementos[1] + "|" + elementos[2] + "|" + elementos[3] + "\n";
    }

    // Sobrescribir el archivo completo con el contenido actualizado
    std::ofstream archivoSalida("credenciales.txt");

    if (!archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura." << std::endl;
        return;
    }

    archivoSalida << nuevoContenido;
    archivoSalida.close();

    if (usuarioEncontrado) {
        std::cout << "Cantidad de intentos reiniciada con éxito." << std::endl;
    } else {
        std::cerr << "Usuario no encontrado en el archivo." << std::endl;
    }
}

int obtenerIntentos(string nombreUsuario){
    ifstream archivo("credenciales.txt");
    int credencialesValidas=-1;
    int intentos;
    string linea;
    while (getline(archivo, linea) && credencialesValidas==-1){
        istringstream ss(linea);
        string elemento;
        vector<std::string> elementos;

        while (getline(ss, elemento, '|')) {
            elementos.push_back(elemento);
        }

        if (elementos.size() == 4)  {
            if (nombreUsuario==elementos[0]){
                intentos=stoi(elementos[3]);
            }
        }
    }

    archivo.close();
    return intentos;
}

