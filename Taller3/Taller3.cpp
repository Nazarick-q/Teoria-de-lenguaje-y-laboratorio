/*
 * TALLER INTEGRADOR: VALIDADOR DE EXPRESIONES MATEMÁTICAS AVANZADAS
 * Teoría de Lenguajes y Laboratorio - Docente: Luz Andrea Páez Martínez
 *
 * El programa aplica tres niveles de validación en cascada:
 *   1) Expresiones Regulares  -> ¿los caracteres usados son válidos?
 *   2) Autómata de Pila       -> ¿los símbolos de agrupación están balanceados?
 *   3) Análisis estructural   -> ¿la secuencia de números/operadores tiene sentido?
 *
 * Compilar:  g++ -std=c++17 -Wall -o validador validador.cpp
 * Ejecutar:  ./validador
 */

#include <iostream>
#include <string>
#include <regex>
#include <stack>
#include <vector>
#include <cctype>

using namespace std;

// ============================================================
//  ESTRUCTURAS DE APOYO
// ============================================================

// Resultado genérico de una validación, con la información
// que la empresa pidió mostrar cuando algo falla.
struct ResultadoValidacion {
    bool valido = true;
    string tipoError;
    string simboloEncontrado;
    string simboloEsperado;
    int posicion = -1; // 1-based, tal como en el ejemplo del taller
};

enum class TipoToken { NUMERO, OPERADOR, APERTURA, CIERRE };

struct Token {
    TipoToken tipo;
    string valor;
    int posicion; // posición (1-based) del primer carácter del token
};

// ============================================================
//  FUNCIONES BÁSICAS DE CLASIFICACIÓN DE CARACTERES
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

// Indica si un símbolo de apertura corresponde al símbolo de cierre dado.
bool coinciden(char apertura, char cierre) {
    return (apertura == '(' && cierre == ')') ||
           (apertura == '[' && cierre == ']') ||
           (apertura == '{' && cierre == '}');
}

// Devuelve el carácter de cierre que le correspondería a una apertura.
char cierreEsperado(char apertura) {
    if (apertura == '(') return ')';
    if (apertura == '[') return ']';
    if (apertura == '{') return '}';
    return '?';
}

// ============================================================
//  NIVEL 1: VALIDACIÓN CON EXPRESIONES REGULARES
// ============================================================
// Se apoya en una expresión regular para reconocer el conjunto de
// caracteres permitido: dígitos, operadores, símbolos de agrupación
// y espacios. Si la expresión completa no calza con el patrón,
// se recorre carácter a carácter para reportar el primer símbolo
// no permitido y su posición.

ResultadoValidacion validarCaracteres(const string& expresion) {
    ResultadoValidacion resultado;

    // Patrón: uno o más caracteres pertenecientes al conjunto permitido.
    // Nota: el '-' se coloca al final de la clase de caracteres (ahí es
    // siempre literal) para evitar el error "Invalid range in bracket
    // expression" que lanza la implementación de <regex> de MSVC cuando
    // se escapa con '\-' dentro de la clase.
    static const regex patronValido(R"(^[0-9+*/\(\)\[\]\{\}\s-]*$)");

    if (regex_match(expresion, patronValido)) {
        resultado.valido = true;
        return resultado;
    }

    // La expresión completa no es válida: buscamos el primer carácter
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

    // No debería llegar aquí, pero por seguridad:
    resultado.valido = false;
    resultado.tipoError = "Caracter no permitido";
    return resultado;
}

// ============================================================
//  NIVEL 2: VALIDACIÓN CON AUTÓMATA DE PILA (balanceo de símbolos)
// ============================================================
// Simula el comportamiento de un autómata de pila: apila cada símbolo
// de apertura y, al encontrar un cierre, verifica que corresponda con
// el tope de la pila. Al final la pila debe quedar vacía.

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
        // números, operadores y espacios no afectan la pila
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
//  TOKENIZADOR (usado por la validación estructural)
// ============================================================
// Agrupa dígitos consecutivos en un solo token NUMERO y clasifica
// el resto de caracteres no-espacio como OPERADOR, APERTURA o CIERRE.

vector<Token> tokenizar(const string& expresion) {
    vector<Token> tokens;
    size_t i = 0;
    size_t n = expresion.size();

    while (i < n) {
        char c = expresion[i];

        if (esEspacio(c)) {
            ++i;
            continue;
        }

        if (esNumero(c)) {
            size_t inicio = i;
            string numero;
            while (i < n && esNumero(expresion[i])) {
                numero += expresion[i];
                ++i;
            }
            tokens.push_back({TipoToken::NUMERO, numero, static_cast<int>(inicio) + 1});
            continue;
        }

        if (esOperador(c)) {
            tokens.push_back({TipoToken::OPERADOR, string(1, c), static_cast<int>(i) + 1});
            ++i;
            continue;
        }

        if (esApertura(c)) {
            tokens.push_back({TipoToken::APERTURA, string(1, c), static_cast<int>(i) + 1});
            ++i;
            continue;
        }

        if (esCierre(c)) {
            tokens.push_back({TipoToken::CIERRE, string(1, c), static_cast<int>(i) + 1});
            ++i;
            continue;
        }

        // Caracter desconocido: ya debió haber sido detectado en el nivel 1,
        // pero lo saltamos para no romper el tokenizador.
        ++i;
    }

    return tokens;
}

// ============================================================
//  NIVEL 3: VALIDACIÓN DE LA ESTRUCTURA MATEMÁTICA
// ============================================================
// Recorre la secuencia de tokens verificando que el "tipo anterior"
// y el "tipo actual" formen una combinación válida. Esto detecta:
//   - dos operadores consecutivos
//   - un operador al inicio
//   - un operador al final
//   - dos números consecutivos sin operador
// y de paso también combinaciones igual de inválidas como ")3".
// Excepción: un cierre seguido de una apertura, ej. "(12)(21)", se
// acepta como multiplicación implícita entre grupos (equivale a "*").

ResultadoValidacion validarEstructura(const string& expresion) {
    ResultadoValidacion resultado;
    vector<Token> tokens = tokenizar(expresion);

    if (tokens.empty()) {
        resultado.valido = false;
        resultado.tipoError = "Estructura invalida";
        resultado.simboloEncontrado = "(expresion vacia)";
        resultado.simboloEsperado = "una expresion con al menos un numero";
        resultado.posicion = 1;
        return resultado;
    }

    // "anterior" indica qué tipo de token puede terminar la expresión
    // o preceder al siguiente token: NUMERO_O_CIERRE, OPERADOR, APERTURA, NINGUNO(inicio)
    TipoToken* anterior = nullptr;
    TipoToken anteriorValor;
    bool hayAnterior = false;

    for (size_t idx = 0; idx < tokens.size(); ++idx) {
        const Token& t = tokens[idx];

        if (!hayAnterior) {
            // Primer token de la expresión.
            if (t.tipo == TipoToken::OPERADOR) {
                resultado.valido = false;
                resultado.tipoError = "Operador al inicio de la expresion";
                resultado.simboloEncontrado = t.valor;
                resultado.simboloEsperado = "un numero o un simbolo de apertura";
                resultado.posicion = t.posicion;
                return resultado;
            }
            if (t.tipo == TipoToken::CIERRE) {
                resultado.valido = false;
                resultado.tipoError = "Estructura invalida";
                resultado.simboloEncontrado = t.valor;
                resultado.simboloEsperado = "un numero o un simbolo de apertura";
                resultado.posicion = t.posicion;
                return resultado;
            }
        } else {
            TipoToken prev = anteriorValor;

            if (prev == TipoToken::NUMERO || prev == TipoToken::CIERRE) {
                // Después de un número o un cierre puede venir un
                // operador, un cierre, o (multiplicación implícita)
                // un número o una apertura.
                if (t.tipo == TipoToken::NUMERO) {
                    if (prev == TipoToken::CIERRE) {
                        // Multiplicacion implicita cierre+numero: ")3" es
                        // valido y equivale a ")*3", ej: (12)3 == (12)*3
                    } else {
                        // prev == NUMERO: dos numeros pegados sin operador
                        // (aqui NO aplica multiplicacion implicita, ya que
                        // "25 8" no es una notacion matematica valida).
                        resultado.valido = false;
                        resultado.tipoError = "Dos numeros consecutivos sin operador";
                        resultado.simboloEncontrado = t.valor;
                        resultado.simboloEsperado = "un operador (+ - * /)";
                        resultado.posicion = t.posicion;
                        return resultado;
                    }
                }
                if (t.tipo == TipoToken::APERTURA) {
                    // Multiplicacion implicita entre numero+apertura o
                    // cierre+apertura: "12(3)" y "(12)(21)" son validos y
                    // equivalen a "12*(3)" y "(12)*(21)".
                }
            } else {
                // prev == OPERADOR o prev == APERTURA
                if (t.tipo == TipoToken::OPERADOR) {
                    resultado.valido = false;
                    resultado.tipoError = (prev == TipoToken::OPERADOR)
                        ? "Dos operadores consecutivos"
                        : "Operador inmediatamente despues de un simbolo de apertura";
                    resultado.simboloEncontrado = t.valor;
                    resultado.simboloEsperado = "un numero o un simbolo de apertura";
                    resultado.posicion = t.posicion;
                    return resultado;
                }
                if (t.tipo == TipoToken::CIERRE) {
                    resultado.valido = false;
                    resultado.tipoError = "Grupo vacio o cierre inesperado";
                    resultado.simboloEncontrado = t.valor;
                    resultado.simboloEsperado = "un numero o un simbolo de apertura";
                    resultado.posicion = t.posicion;
                    return resultado;
                }
            }
        }

        anteriorValor = t.tipo;
        hayAnterior = true;
        (void)anterior;
    }

    // El último token no puede ser un operador ni una apertura sin cerrar
    // (el balanceo ya se validó en el nivel 2, así que solo revisamos operador).
    if (anteriorValor == TipoToken::OPERADOR) {
        resultado.valido = false;
        resultado.tipoError = "Operador al final de la expresion";
        resultado.simboloEncontrado = tokens.back().valor;
        resultado.simboloEsperado = "un numero o un simbolo de apertura";
        resultado.posicion = tokens.back().posicion;
        return resultado;
    }

    resultado.valido = true;
    return resultado;
}

// ============================================================
//  PRESENTACIÓN DE RESULTADOS
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

// Ejecuta las tres validaciones en cascada sobre una expresión,
// deteniéndose en el primer nivel que falle (tal como pide el flujo del taller).
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