#include "gestionClientes.h"

using namespace std;

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
            mensajeAlCliente="No fue posible encontrar la traduccion para: " + palabra;
        }

        send(s,mensajeAlCliente.c_str(),mensajeAlCliente.size(),0);
        archivo.close();
    }

}

string devuelveBloqueados(vector<string> bloqueados){

    string listaBloqueados;

    if (bloqueados.empty()){
        listaBloqueados="-1";
    }else{

        for (string usuario:bloqueados){
            listaBloqueados.append(usuario+"\n");
        }
    }



    return listaBloqueados;
}
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

int desbloquear(std::vector<std::string>& usuarios, const std::string& nombreUsuario) {

    int encontrado=0;
    auto it = std::find(usuarios.begin(), usuarios.end(), nombreUsuario);
    if (it != usuarios.end()) {
        usuarios.erase(it);
        encontrado = 1;
    }

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


void aumentarIntentos( string nombre,  string contrasena) {
    ifstream inputFile("credenciales.txt");
    ofstream outputFile("temp.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error al abrir archivos." << endl;
        return;
    }

    string linea;
    while (getline(inputFile, linea)) {
        istringstream iss(linea);
        string nombreUsuario, password, rol;
        int intentos;
        char delimitador = '|';

        if (getline(iss, nombreUsuario, delimitador) &&
            getline(iss, password, delimitador) &&
            getline(iss, rol, delimitador) &&
            (iss >> intentos)) {
            if (nombreUsuario == nombre && password != contrasena && rol!="ADMIN") {
                intentos++;
            }
            outputFile << nombreUsuario << delimitador << password << delimitador << rol << delimitador << intentos << endl;
        } else {
            cerr << "Error en el formato de la línea: " << linea << endl;
        }
    }

    inputFile.close();
    outputFile.close();

    // Reemplaza el archivo original con el archivo temporal
    remove("credenciales.txt");
    rename("temp.txt", "credenciales.txt");
}


void reiniciarIntentos(string usuario) {
    std::ifstream inputFile("credenciales.txt");
    std::ofstream outputFile("temp.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error al abrir archivos." << std::endl;
        return;
    }

    std::string linea;
    while (std::getline(inputFile, linea)) {
        std::istringstream iss(linea);
        std::string nombreUsuario, password, rol;
        int intentos;
        char delimitador = '|';

        if (std::getline(iss, nombreUsuario, delimitador) &&
            std::getline(iss, password, delimitador) &&
            std::getline(iss, rol, delimitador) &&
            (iss >> intentos)) {
            if (nombreUsuario == usuario) {
                intentos = 0;
            }
            outputFile << nombreUsuario << delimitador << password << delimitador << rol << delimitador << intentos << std::endl;
        } else {
            std::cerr << "Error en el formato de la línea: " << linea << std::endl;
        }
    }

    inputFile.close();
    outputFile.close();

    // Reemplaza el archivo original con el archivo temporal
    std::remove("credenciales.txt");
    std::rename("temp.txt", "credenciales.txt");
}

/*
void reestablecerIntentos(string nombre) {
    ifstream inputFile("credenciales.txt");
    ofstream outputFile("temp.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error al abrir archivos." << endl;
        return;
    }

    string linea;
    while (getline(inputFile, linea)) {
        istringstream iss(linea);
        string nombreUsuario, password, rol;
        int intentos;
        char delimitador = '|';

        if (getline(iss, nombreUsuario, delimitador) &&
            getline(iss, password, delimitador) &&
            getline(iss, rol, delimitador) &&
            (iss >> intentos)) {
            if (nombreUsuario == nombre) {
                intentos=0;
            }
            outputFile << nombreUsuario << delimitador << password << delimitador << rol << delimitador << intentos << endl;
        } else {
            cerr << "Error en el formato de la línea: " << linea << endl;
        }
    }

    inputFile.close();
    outputFile.close();

    // Reemplaza el archivo original con el archivo temporal
    remove("credenciales.txt");
    rename("temp.txt", "credenciales.txt");
}
*/



/*
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

*/
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
