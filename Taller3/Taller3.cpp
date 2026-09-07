/*
 * TALLER INTEGRADOR: VALIDADOR DE EXPRESIONES MATEMATICAS AVANZADAS
 * Teoria de Lenguajes y Laboratorio - Docente: Luz Andrea Paez Martinez
 *
 * El programa aplica tres niveles de validacion en cascada:
 *   1) Expresiones Regulares  -> los caracteres usados son validos?
 *   2) Automata de Pila       -> los simbolos de agrupacion estan balanceados?
 *   3) Analisis estructural   -> la secuencia de numeros/operadores tiene sentido?
 */

#include <iostream>
#include <string>
#include <regex>
#include <stack>
#include <cctype>

using namespace std;

// ============================================================
//  ESTRUCTURAS DE APOYO
// ============================================================

// Resultado generico de una validacion, con la informacion
// que la empresa pidio mostrar cuando algo falla.
struct ResultadoValidacion {
    bool valido = true;
    string tipoError;
    string simboloEncontrado;
    string simboloEsperado;
    int posicion = -1; // 1-based
};

// Representa el "tipo" del ultimo simbolo significativo ya procesado,
// que actua como estado del automata al recorrer la expresion.
enum class TipoToken { NUMERO, OPERADOR, APERTURA, CIERRE };

// ============================================================
//  FUNCIONES BASICAS DE CLASIFICACION DE CARACTERES
// ============================================================

bool esNumero(char caracter) {
    return isdigit(static_cast<unsigned char>(caracter));
}

bool esOperador(char caracter) {
    return caracter == '+' || caracter == '-' || caracter == '*' || caracter == '/';
}

bool esApertura(char caracter) {
    return caracter == '(' || caracter == '[' || caracter == '{';
}

bool esCierre(char caracter) {
    return caracter == ')' || caracter == ']' || caracter == '}';
}

bool esEspacio(char caracter) {
    return caracter == ' ' || caracter == '\t';
}

// Indica si un simbolo de apertura corresponde al simbolo de cierre dado.
bool coinciden(char apertura, char cierre) {
    return (apertura == '(' && cierre == ')') ||
           (apertura == '[' && cierre == ']') ||
           (apertura == '{' && cierre == '}');
}

// Devuelve el caracter de cierre que le corresponderia a una apertura.
char cierreEsperado(char apertura) {
    if (apertura == '(') return ')';
    if (apertura == '[') return ']';
    if (apertura == '{') return '}';
    return '?';
}

// ============================================================
//  NIVEL 1: VALIDACION CON EXPRESIONES REGULARES
// ============================================================
// Se apoya en una expresion regular para reconocer el conjunto de
// caracteres permitido: digitos, operadores, simbolos de agrupacion
// y espacios. Si la expresion completa no calza con el patron,
// se recorre caracter a caracter para reportar el primer simbolo
// no permitido y su posicion.

ResultadoValidacion validarCaracteres(const string& expresion) {
    ResultadoValidacion resultado;

    // Patron: uno o mas caracteres pertenecientes al conjunto permitido.
    // Nota: el '-' se coloca al final de la clase de caracteres (ahi es
    // siempre literal) para evitar el error "Invalid range in bracket
    // expression" que lanza la implementacion de <regex> de MSVC cuando
    // se escapa con '\-' dentro de la clase.
    static const regex patronValido(R"(^[0-9+*/\(\)\[\]\{\}\s-]*$)");

    if (regex_match(expresion, patronValido)) {
        resultado.valido = true;
        return resultado;
    }

    // La expresion completa no es valida: buscamos el primer caracter
    // que no pertenezca al conjunto permitido para reportarlo.
    for (size_t i = 0; i < expresion.size(); ++i) {
        char c = expresion[i];
        bool permitido = esNumero(c) || esOperador(c) || esApertura(c) ||
                          esCierre(c) || esEspacio(c);
        if (!permitido) {
            resultado.valido = false;
            resultado.tipoError = "Caracter no permitido";
            resultado.simboloEncontrado = string(1, c);
            resultado.simboloEsperado = "numero, operador (+ - * /) o simbolo de agrupacion";
            resultado.posicion = static_cast<int>(i) + 1; // 1-based
            return resultado;
        }
    }

    // No deberia llegar aqui, pero por seguridad:
    resultado.valido = false;
    resultado.tipoError = "Caracter no permitido";
    return resultado;
}

// ============================================================
//  NIVEL 2: VALIDACION CON AUTOMATA DE PILA (balanceo de simbolos)
// ============================================================
// Simula el comportamiento de un automata de pila: apila cada simbolo
// de apertura y, al encontrar un cierre, verifica que corresponda con
// el tope de la pila. Al final la pila debe quedar vacia.

ResultadoValidacion validarBalanceo(const string& expresion) {
    ResultadoValidacion resultado;
    stack<pair<char, int>> pila; // (simbolo, posicion 1-based)

    for (size_t i = 0; i < expresion.size(); ++i) {
        char c = expresion[i];
        int pos = static_cast<int>(i) + 1;

        if (esApertura(c)) {
            pila.push({c, pos});
        } else if (esCierre(c)) {
            if (pila.empty()) {
                // Cierre sin apertura previa.
                resultado.valido = false;
                resultado.tipoError = "Simbolos de agrupacion incorrectamente balanceados";
                resultado.simboloEncontrado = string(1, c);
                resultado.simboloEsperado = "ningun simbolo de cierre (no hay apertura pendiente)";
                resultado.posicion = pos;
                return resultado;
            }

            char apertura = pila.top().first;
            if (!coinciden(apertura, c)) {
                resultado.valido = false;
                resultado.tipoError = "Simbolos de agrupacion incorrectamente balanceados";
                resultado.simboloEncontrado = string(1, c);
                resultado.simboloEsperado = string(1, cierreEsperado(apertura));
                resultado.posicion = pos;
                return resultado;
            }
            pila.pop();
        }
        // numeros, operadores y espacios no afectan la pila
    }

    if (!pila.empty()) {
        // Quedaron aperturas sin cerrar.
        char apertura = pila.top().first;
        int posApertura = pila.top().second;
        resultado.valido = false;
        resultado.tipoError = "Simbolos de agrupacion incorrectamente balanceados";
        resultado.simboloEncontrado = "fin de la expresion";
        resultado.simboloEsperado = string(1, cierreEsperado(apertura)) +
            " (para cerrar '" + apertura + "' abierto en la posicion " + to_string(posApertura) + ")";
        resultado.posicion = static_cast<int>(expresion.size());
        return resultado;
    }

    resultado.valido = true;
    return resultado;
}

// ============================================================
//  NIVEL 3: VALIDACION DE LA ESTRUCTURA MATEMATICA
// ============================================================
// A diferencia de los niveles anteriores, aqui NO se construye una
// lista de tokens: se recorre la expresion una sola vez, caracter a
// caracter, guardando en una variable de estado el "tipo" del ultimo
// simbolo significativo visto (numero, operador, apertura o cierre).
// Esa variable de estado hace las veces de automata: en cada paso,
// el tipo anterior + el caracter actual determinan si la transicion
// es valida o si se rechaza la expresion.
//
// Detecta:
//   - dos operadores consecutivos
//   - un operador al inicio
//   - un operador al final
//   - dos numeros consecutivos sin operador
//   - un cierre inesperado o un grupo vacio
// Excepcion (multiplicacion implicita, aceptada como si hubiera "*"):
//   numero+apertura ("12(3)"), cierre+numero ("(3)12") y
//   cierre+apertura ("(12)(21)").

ResultadoValidacion validarEstructura(const string& expresion) {
    ResultadoValidacion resultado;

    bool hayAnterior = false;      // ya vimos algun simbolo significativo?
    TipoToken anterior{};          // tipo del ultimo simbolo significativo
    string ultimoValor;            // texto del ultimo simbolo (para el error final)
    int ultimaPosicion = 0;        // posicion 1-based del ultimo simbolo

    size_t i = 0;
    size_t n = expresion.size();

    while (i < n) {
        char c = expresion[i];

        if (esEspacio(c)) {
            ++i;
            continue;
        }

        int posActual = static_cast<int>(i) + 1; // 1-based

        if (esNumero(c)) {
            // Agrupamos todos los digitos consecutivos: son un solo numero.
            size_t inicio = i;
            while (i < n && esNumero(expresion[i])) ++i;
            string valor = expresion.substr(inicio, i - inicio);

            if (hayAnterior && (anterior == TipoToken::NUMERO || anterior == TipoToken::CIERRE)) {
                if (anterior == TipoToken::NUMERO) {
                    // "25 8": dos numeros pegados sin operador -> invalido.
                    resultado.valido = false;
                    resultado.tipoError = "Dos numeros consecutivos sin operador";
                    resultado.simboloEncontrado = valor;
                    resultado.simboloEsperado = "un operador (+ - * /)";
                    resultado.posicion = posActual;
                    return resultado;
                }
                // anterior == CIERRE: multiplicacion implicita ")3" -> valido.
            }

            anterior = TipoToken::NUMERO;
            ultimoValor = valor;
            ultimaPosicion = posActual;
            hayAnterior = true;
            continue;
        }

        if (esOperador(c)) {
            if (!hayAnterior) {
                resultado.valido = false;
                resultado.tipoError = "Operador al inicio de la expresion";
                resultado.simboloEncontrado = string(1, c);
                resultado.simboloEsperado = "un numero o un simbolo de apertura";
                resultado.posicion = posActual;
                return resultado;
            }
            if (anterior == TipoToken::OPERADOR || anterior == TipoToken::APERTURA) {
                resultado.valido = false;
                resultado.tipoError = (anterior == TipoToken::OPERADOR)
                    ? "Dos operadores consecutivos"
                    : "Operador inmediatamente despues de un simbolo de apertura";
                resultado.simboloEncontrado = string(1, c);
                resultado.simboloEsperado = "un numero o un simbolo de apertura";
                resultado.posicion = posActual;
                return resultado;
            }

            anterior = TipoToken::OPERADOR;
            ultimoValor = string(1, c);
            ultimaPosicion = posActual;
            hayAnterior = true;
            ++i;
            continue;
        }

        if (esApertura(c)) {
            // numero+apertura o cierre+apertura: multiplicacion implicita,
            // siempre valido (no se necesita chequeo aqui).
            anterior = TipoToken::APERTURA;
            ultimoValor = string(1, c);
            ultimaPosicion = posActual;
            hayAnterior = true;
            ++i;
            continue;
        }

        if (esCierre(c)) {
            if (!hayAnterior || anterior == TipoToken::OPERADOR || anterior == TipoToken::APERTURA) {
                resultado.valido = false;
                resultado.tipoError = "Grupo vacio o cierre inesperado";
                resultado.simboloEncontrado = string(1, c);
                resultado.simboloEsperado = "un numero o un simbolo de apertura";
                resultado.posicion = posActual;
                return resultado;
            }

            anterior = TipoToken::CIERRE;
            ultimoValor = string(1, c);
            ultimaPosicion = posActual;
            hayAnterior = true;
            ++i;
            continue;
        }

        // Caracter desconocido: ya debio haber sido detectado en el nivel 1.
        ++i;
    }

    if (!hayAnterior) {
        resultado.valido = false;
        resultado.tipoError = "Estructura invalida";
        resultado.simboloEncontrado = "(expresion vacia)";
        resultado.simboloEsperado = "una expresion con al menos un numero";
        resultado.posicion = 1;
        return resultado;
    }

    // La expresion no puede terminar en un operador.
    if (anterior == TipoToken::OPERADOR) {
        resultado.valido = false;
        resultado.tipoError = "Operador al final de la expresion";
        resultado.simboloEncontrado = ultimoValor;
        resultado.simboloEsperado = "un numero o un simbolo de apertura";
        resultado.posicion = ultimaPosicion;
        return resultado;
    }

    resultado.valido = true;
    return resultado;
}


// ============================================================
//  PRESENTACION DE RESULTADOS
// ============================================================

void mostrarResultado(const string& expresion,
                       const ResultadoValidacion& caracteres,
                       const ResultadoValidacion* balanceo,
                       const ResultadoValidacion* estructura) {
    cout << "======================================\n";
    cout << "   ANALIZADOR DE EXPRESIONES\n";
    cout << "======================================\n\n";
    cout << "Expresion ingresada:\n" << expresion << "\n\n";

    cout << "Validacion de caracteres: " << (caracteres.valido ? "VALIDA" : "INVALIDA") << "\n";
    if (balanceo)
        cout << "Validacion de simbolos:   " << (balanceo->valido ? "VALIDA" : "INVALIDA") << "\n";
    if (estructura)
        cout << "Validacion de estructura: " << (estructura->valido ? "VALIDA" : "INVALIDA") << "\n";
    cout << "\n";

    const ResultadoValidacion* fallo = nullptr;
    if (!caracteres.valido) fallo = &caracteres;
    else if (balanceo && !balanceo->valido) fallo = balanceo;
    else if (estructura && !estructura->valido) fallo = estructura;

    if (fallo) {
        cout << "RESULTADO FINAL:\n";
        cout << "EXPRESION RECHAZADA\n\n";
        cout << "ERROR EN LA POSICION: " << fallo->posicion << "\n\n";
        cout << "TIPO DE ERROR:\n" << fallo->tipoError << "\n\n";
        cout << "SIMBOLO ENCONTRADO:\n" << fallo->simboloEncontrado << "\n\n";
        cout << "SIMBOLO ESPERADO:\n" << fallo->simboloEsperado << "\n";
    } else {
        cout << "RESULTADO FINAL:\n";
        cout << "EXPRESION ACEPTADA\n";
    }
    cout << "\n";
}

// Ejecuta las tres validaciones en cascada sobre una expresion,
// deteniendose en el primer nivel que falle (tal como pide el flujo del taller).
void procesarExpresion(const string& expresion) {
    ResultadoValidacion caracteres = validarCaracteres(expresion);
    if (!caracteres.valido) {
        mostrarResultado(expresion, caracteres, nullptr, nullptr);
        return;
    }

    ResultadoValidacion balanceo = validarBalanceo(expresion);
    if (!balanceo.valido) {
        mostrarResultado(expresion, caracteres, &balanceo, nullptr);
        return;
    }

    ResultadoValidacion estructura = validarEstructura(expresion);
    mostrarResultado(expresion, caracteres, &balanceo, &estructura);
}

// ============================================================
//  PROGRAMA PRINCIPAL
// ============================================================

int main() {
    cout << "=========================================================\n";
    cout << " VALIDADOR DE EXPRESIONES MATEMATICAS AVANZADAS\n";
    cout << " (Regex + Automata de Pila + Validacion Estructural)\n";
    cout << "=========================================================\n";
    cout << "Escriba 'salir' para terminar el programa.\n\n";

    string expresion;
    while (true) {
        cout << "Ingrese una expresion: ";
        if (!getline(cin, expresion)) break;
        if (expresion == "salir") break;
        cout << "\n";
        procesarExpresion(expresion);
        cout << "---------------------------------------------------------\n\n";
    }

    cout << "Programa finalizado.\n";
    return 0;
}