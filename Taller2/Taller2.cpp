#include <iostream>
#include <string>
#include <regex>

using namespace std;

int main()
{
    string nombre;
    string apellido;
    string correo;
    string telefono;
    string documento;

    // Expresiones regulares
    regex nombreRegex(
        "^[A-Za-z]+( [A-Za-z]+)*$"
    );

    regex correoRegex(
        "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"
    );

    regex telefonoRegex(
        "^[0-9]{7,10}$"
    );

    regex documentoRegex(
        "^[0-9]{6,10}$"
    );

    cout << "========================================" << endl;
    cout << "          REGISTRO AL EVENTO" << endl;
    cout << "========================================" << endl;
    cout << endl;


    // NOMBRE
    do
    {
        cout << "Nombre: ";
        getline(cin, nombre);

        if (!regex_match(nombre, nombreRegex))
        {
            cout << "Nombre invalido. Intente nuevamente." << endl;
        }

    } while (!regex_match(nombre, nombreRegex));

    cout << "Nombre valido." << endl;
    cout << endl;


    // APELLIDO
    do
    {
        cout << "Apellido: ";
        getline(cin, apellido);

        if (!regex_match(apellido, nombreRegex))
        {
            cout << "Apellido invalido. Intente nuevamente." << endl;
        }

    } while (!regex_match(apellido, nombreRegex));

    cout << "Apellido valido." << endl;
    cout << endl;


    // CORREO
    do
    {
        cout << "Correo electronico: ";
        getline(cin, correo);

        if (!regex_match(correo, correoRegex))
        {
            cout << "Correo invalido. Intente nuevamente." << endl;
        }

    } while (!regex_match(correo, correoRegex));

    cout << "Correo valido." << endl;
    cout << endl;


    // TELEFONO
    do
    {
        cout << "Telefono: ";
        getline(cin, telefono);

        if (!regex_match(telefono, telefonoRegex))
        {
            cout << "Telefono invalido. Debe tener entre 7 y 10 numeros." << endl;
        }

    } while (!regex_match(telefono, telefonoRegex));

    cout << "Telefono valido." << endl;
    cout << endl;


    // DOCUMENTO
    do
    {
        cout << "Documento: ";
        getline(cin, documento);

        if (!regex_match(documento, documentoRegex))
        {
            cout << "Documento invalido. Debe tener entre 6 y 10 numeros." << endl;
        }

    } while (!regex_match(documento, documentoRegex));

    cout << "Documento valido." << endl;
    cout << endl;


    // RESULTADO
    cout << "========================================" << endl;
    cout << "        REGISTRO COMPLETADO" << endl;
    cout << "========================================" << endl;

    cout << "Nombre: " << nombre << endl;
    cout << "Apellido: " << apellido << endl;
    cout << "Correo: " << correo << endl;
    cout << "Telefono: " << telefono << endl;
    cout << "Documento: " << documento << endl;

    cout << endl;
    cout << "Todos los datos fueron validados correctamente." << endl;

    return 0;
}