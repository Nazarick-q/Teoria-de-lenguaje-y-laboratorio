/*
 * AFD - Ejercicio 3: Registro de un evento
 * La cadena debe tener la secuencia R, C, P en ese orden.
 */

#include <iostream>
#include <string>
using namespace std;

int main() {
    int cantidad;

    cout << "AFD - Registro de evento (secuencia R -> C -> P)\n";
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

            if (letra != 'R' && letra != 'C' && letra != 'P') {
                valida = false;
                break;
            }

            if (estado == 0) {
                if (letra == 'R')
                    estado = 1;
                else
                    estado = 4;
            }
            else if (estado == 1) {
                if (letra == 'C')
                    estado = 2;
                else
                    estado = 4;
            }
            else if (estado == 2) {
                if (letra == 'P')
                    estado = 3;
                else
                    estado = 4;
            }
            else if (estado == 3) {
                if (letra == 'R' || letra == 'C')
                    estado = 3;
                else
                    estado = 4;
            }
            else {
                estado = 4;
            }
        }

        if (!valida) {
            cout << "  -> Cadena con simbolos fuera del alfabeto {R,C,P}. Rechazada.\n";
        }
        else if (estado == 3) {
            cout << "  -> ACEPTADA (se registro, confirmo y participo en orden)\n";
        }
        else {
            cout << "  -> RECHAZADA\n";
        }
    }

    return 0;
}
