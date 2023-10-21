#include "conexion.h"


SOCKET crearSocketCliente() {
    SOCKET clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        cerr << "Error al crear el socket del cliente." << endl;
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
