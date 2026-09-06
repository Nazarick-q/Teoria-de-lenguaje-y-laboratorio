/*
 * AFN - Ejercicio 2: Categoria de un producto
 * Acepta las cadenas que tengan por lo menos una P.
 */

#include <iostream>
#include <string>
using namespace std;

bool simularAFN(string cadena) {
    bool encontroP = false;

    for (int i = 0; i < cadena.length(); i++) {
        if (cadena[i] == 'P') {
            encontroP = true;
        }
    }

    return encontroP;
}

int main() {
    int cantidad;

    cout << "AFN - Categoria de producto (debe contener al menos una P)\n";
    cout << "Ingrese el numero de cadenas a probar: ";
    cin >> cantidad;

    for (int i = 0; i < cantidad; i++) {
        string cadena;
        cout << "Cadena " << i + 1 << ": ";
        cin >> cadena;

        bool valida = true;

        for (int j = 0; j < cadena.length(); j++) {
            if (cadena[j] != 'A' && cadena[j] != 'B' && cadena[j] != 'P') {
                valida = false;
                break;
            }
        }

        if (!valida) {
            cout << "  -> Cadena con simbolos fuera de {A,B,P}. Rechazada.\n";
        }
        else if (simularAFN(cadena)) {
            cout << "  -> ACEPTADA (pertenece a la categoria)\n";
        }
        else {
            cout << "  -> RECHAZADA\n";
        }
    }

    return 0;
}
