#include "menu.h"

void menuConsulta(SOCKET s){
    int opcion;
    do{
        cout << "1- Traducir |2- Cerrar sesion" << endl;

        cout << "Ingresar opcion: " << endl;
        cin >> opcion;
        send(s,(char*)&opcion,sizeof(opcion),0);

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
            cout << "1-Alta Usuarios | 2-Desbloqueo | Ingrese '/salir' o 0 para volver al menu principal "<< endl;
             // cout << "1-Alta Usuarios | 2-Desbloqueo" << endl;
            cout << "----Ingresar opcion:---- " << endl;
            cin >> opcionstr;

            if (opcionstr=="/salir"){
                opcionMenuUsuarios=0;
            }
            else{
                opcionMenuUsuarios= stoi(opcionstr);
            }

            send(s,(const char*)&opcionMenuUsuarios,sizeof(opcionMenuUsuarios),0);

            //ALTA
            if (opcionMenuUsuarios==1){
                cout << "-------ALTA USUARIOS--------" << endl;
                //enviarAltaUsuarios(s);
                enviarNombreUsuario(s);
                enviarContrasena(s);
                char buffer[100];
                recibir(s,buffer,sizeof(buffer));
                cout << buffer << endl;

            }
            //DESBLOQUEO
            if (opcionMenuUsuarios==2){

                char listaBloqueados[256];
                int tam=recibir(s,listaBloqueados,sizeof(listaBloqueados));
                string listaBloqueadosStr= string(listaBloqueados,tam);
                char buffer[100];

                if (listaBloqueadosStr=="-1"){
                    recibir(s,buffer,sizeof(buffer));
                    cout << buffer << endl;
                }else{
                    cout << "Lista de bloqueados: \n" << listaBloqueados << endl;
                    cout << "Ingresar '/salir' si no quiere desbloquear ningun usuario" << endl;
                    enviarNombreUsuario(s);
                    recibir(s,buffer,sizeof(buffer));
                    cout << buffer << endl;
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

void mostrarOpciones(SOCKET s, string rol){

    if (rol=="ADMIN"){
       menuAdmin(s);
    }

    if (rol=="CONSULTA"){
        menuConsulta(s);
    }
}

