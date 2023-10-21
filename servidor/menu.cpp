#include "menu.h"

void menuConsulta(SOCKET s,string usuario){

    int opcion;
    do{
        recv(s,(char*)&opcion,sizeof(opcion),0);

        if (opcion==1){
            string traduccion=  pedirTraduccion(s);
            cout << traduccion << endl;
            pasarAMinusculas(traduccion);
            enviaTraduccion(s,traduccion);

        }
        if (opcion==2){
            closesocket(s);
            string mensaje="Cierre de sesion – usuario: " + usuario;
            serverLog(mensaje);
        }
    }while (opcion!=2);

}


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
        pasarAMinusculas(traduccion);
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

                    enviar(s,listaBloqueados);
                    if (listaBloqueados=="-1"){
                        string mensaje="No se encontraron usuarios bloqueados";
                        enviar(s,mensaje);
                    }else{
                        char nombreUsuario[30];
                        recibir(s,nombreUsuario,sizeof(nombreUsuario));
                        int desbloquearInt=desbloquear(bloqueados,string(nombreUsuario));

                        cout <<"desbloquearInt:" << desbloquearInt << endl;
                        if (desbloquearInt==1){
                            enviar(s,string(nombreUsuario)+ " desbloqueado correctamente");
                            reiniciarIntentos(nombreUsuario);

                        }
                        else if (string(nombreUsuario)=="/salir"){
                            enviar(s,"No ha desbloqueado a ningun usuario. Volviendo al menu de usuarios");
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
        serverLog("Cierre de sesion – usuario: " + usuario);
    }

    }while (opcion!=4);



}
