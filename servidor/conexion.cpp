#include "conexion.h"


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

    //serverLog("=======Inicia Servidor=======");
    //serverLog("Socket creado. Puerto de escucha: " + to_string(puerto));

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
