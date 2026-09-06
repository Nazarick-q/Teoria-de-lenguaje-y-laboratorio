/*
 * AFN - Ejercicio 1: Codigo de inscripcion
 * La cadena debe comenzar con A.
 */

#include <iostream>
#include <string>
using namespace std;

bool simularAFN(string cadena) {
    int estado = 0;

    for (int i = 0; i < cadena.length(); i++) {
        char letra = cadena[i];

        if (estado == 0) {
            if (letra == 'A')
                estado = 1;
            else
                return false;
        }
        else {
            if (letra != 'A' && letra != 'B' && letra != 'C')
                return false;
        }
    }

    if (estado == 1)
        return true;

    return false;
}

int main() {
    int cantidad;

    cout << "AFN - Codigo de inscripcion (debe comenzar con A)\n";
    cout << "Ingrese el numero de cadenas a probar: ";
    cin >> cantidad;

    for (int i = 0; i < cantidad; i++) {
        string cadena;
        cout << "Cadena " << i + 1 << ": ";
        cin >> cadena;

        bool valida = !cadena.empty();

        for (int j = 0; j < cadena.length(); j++) {
            if (cadena[j] != 'A' && cadena[j] != 'B' && cadena[j] != 'C') {
                valida = false;
                break;
            }
        }

        if (!valida) {
            cout << "  -> Cadena vacia o con simbolos fuera de {A,B,C}. Rechazada.\n";
        }
        else if (simularAFN(cadena)) {
            cout << "  -> ACEPTADA (comienza con A)\n";
        }
        else {
            cout << "  -> RECHAZADA\n";
        }
    }

    return 0;
}
