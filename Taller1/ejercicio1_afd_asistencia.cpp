/*
 * AFD - Ejercicio 1: Registro de asistencia
 * Alfabeto: {P, A}
 * Acepta las cadenas que tengan por lo menos una P.
 */

#include <iostream>
#include <string>
using namespace std;

int main() {
    int cantidad;

    cout << "AFD - Registro de asistencia (al menos una P)\n";
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

            if (letra != 'P' && letra != 'A') {
                valida = false;
                break;
            }

            if (estado == 0) {
                if (letra == 'P') {
                    estado = 1;
                }
            }
        }

        if (!valida) {
            cout << "  -> Cadena con simbolos fuera del alfabeto {P,A}. Rechazada.\n";
        }
        else if (estado == 1) {
            cout << "  -> ACEPTADA (el estudiante estuvo presente al menos una vez)\n";
        }
        else {
            cout << "  -> RECHAZADA\n";
        }
    }

    return 0;
}
