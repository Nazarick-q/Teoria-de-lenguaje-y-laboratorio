/*
 * AFD - Ejercicio 2: Codigo de acceso
 * Alfabeto: {0, 1}
 * Acepta las cadenas que terminan en 01.
 */

#include <iostream>
#include <string>
using namespace std;

int main() {
    int cantidad;

    cout << "AFD - Codigo de acceso (debe terminar en 01)\n";
    cout << "Ingrese el numero de cadenas a probar: ";
    cin >> cantidad;

    for (int i = 0; i < cantidad; i++) {
        string cadena;
        cout << "Cadena " << i + 1 << ": ";
        cin >> cadena;

        int estado = 0;
        bool valida = true;

        for (int j = 0; j < cadena.length(); j++) {
            char letra = cadena[j];

            if (letra != '0' && letra != '1') {
                valida = false;
                break;
            }

            if (estado == 0) {
                if (letra == '0')
                    estado = 1;
                else
                    estado = 0;
            }
            else if (estado == 1) {
                if (letra == '0')
                    estado = 1;
                else
                    estado = 2;
            }
            else {
                if (letra == '0')
                    estado = 1;
                else
                    estado = 0;
            }
        }

        if (!valida) {
            cout << "  -> Cadena con simbolos fuera del alfabeto {0,1}. Rechazada.\n";
        }
        else if (estado == 2) {
            cout << "  -> ACEPTADA (el codigo termina en 01)\n";
        }
        else {
            cout << "  -> RECHAZADA\n";
        }
    }

    return 0;
}
