/*
 * AFN - Ejercicio 3: Actividad especial
 * La cadena debe comenzar con A o con B.
 */

#include <iostream>
#include <string>
using namespace std;

bool simularAFN(string cadena) {
    if (cadena.length() == 0)
        return false;

    if (cadena[0] != 'A' && cadena[0] != 'B')
        return false;

    for (int i = 0; i < cadena.length(); i++) {
        if (cadena[i] != 'A' && cadena[i] != 'B' && cadena[i] != 'C')
            return false;
    }

    return true;
}

int main() {
    int cantidad;

    cout << "AFN - Registro actividad especial (debe iniciar con A o B)\n";
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
            cout << "  -> ACEPTADA (comienza con A o con B)\n";
        }
        else {
            cout << "  -> RECHAZADA\n";
        }
    }

    return 0;
}
