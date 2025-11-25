#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// ==================== ESTRUCTURAS DE DATOS ====================
struct Miembro {
    int id;
    string nombre;
    string fechaNacimiento;
    string genero;
    int nivel;
    
    Miembro(int i, string n, string f, string g, int lvl = 0) 
        : id(i), nombre(n), fechaNacimiento(f), genero(g), nivel(lvl) {}
    
    void mostrar() const {
        cout << "ID: " << setw(3) << id << " | " 
             << setw(20) << left << nombre << " | "
             << "Nivel: " << setw(2) << nivel << " | "
             << "Nac: " << fechaNacimiento << " | "
             << "Género: " << genero;
    }
};

struct Nodo {
    Miembro dato;
    shared_ptr<Nodo> izquierdo;
    shared_ptr<Nodo> derecho;
    shared_ptr<Nodo> padre;
    vector<shared_ptr<Nodo>> hijos;
    int altura;
    
    Nodo(Miembro m) : dato(m), altura(1) {}
};

// ==================== CLASE PRINCIPAL SIMPLIFICADA ====================
class ArbolGenealogico {
private:
    shared_ptr<Nodo> raiz;
    int siguienteID;
    
    // ==================== FUNCIONES AUXILIARES ====================
    int obtenerAltura(shared_ptr<Nodo> n) const {
        return n ? n->altura : 0;
    }
    
    int obtenerBalance(shared_ptr<Nodo> n) const {
        return n ? obtenerAltura(n->derecho) - obtenerAltura(n->izquierdo) : 0;
    }
    
    void actualizarAltura(shared_ptr<Nodo> n) {
        if (n) {
            n->altura = 1 + max(obtenerAltura(n->izquierdo), obtenerAltura(n->derecho));
        }
    }
    
    // ==================== ROTACIONES AVL ====================
    shared_ptr<Nodo> rotarDerecha(shared_ptr<Nodo> y) {
        if (!y || !y->izquierdo) return y;
        
        auto x = y->izquierdo;
        auto T2 = x->derecho;
        
        x->derecho = y;
        y->izquierdo = T2;
        
        actualizarAltura(y);
        actualizarAltura(x);
        
        return x;
    }
    
    shared_ptr<Nodo> rotarIzquierda(shared_ptr<Nodo> x) {
        if (!x || !x->derecho) return x;
        
        auto y = x->derecho;
        auto T2 = y->izquierdo;
        
        y->izquierdo = x;
        x->derecho = T2;
        
        actualizarAltura(x);
        actualizarAltura(y);
        
        return y;
    }
    
    // ==================== BALANCEO ====================
    shared_ptr<Nodo> balancear(shared_ptr<Nodo> nodo) {
        if (!nodo) return nodo;
        
        actualizarAltura(nodo);
        int balance = obtenerBalance(nodo);
        
        // Caso izquierda-izquierda
        if (balance < -1 && obtenerBalance(nodo->izquierdo) <= 0) {
            return rotarDerecha(nodo);
        }
        
        // Caso derecha-derecha
        if (balance > 1 && obtenerBalance(nodo->derecho) >= 0) {
            return rotarIzquierda(nodo);
        }
        
        // Caso izquierda-derecha
        if (balance < -1 && obtenerBalance(nodo->izquierdo) > 0) {
            nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
            return rotarDerecha(nodo);
        }
        
        // Caso derecha-izquierda
        if (balance > 1 && obtenerBalance(nodo->derecho) < 0) {
            nodo->derecho = rotarDerecha(nodo->derecho);
            return rotarIzquierda(nodo);
        }
        
        return nodo;
    }
    
    // ==================== OPERACIONES ABB ====================
    shared_ptr<Nodo> insertarRecursivo(shared_ptr<Nodo> nodo, Miembro miembro) {
        if (!nodo) {
            return make_shared<Nodo>(miembro);
        }
        
        if (miembro.id < nodo->dato.id) {
            nodo->izquierdo = insertarRecursivo(nodo->izquierdo, miembro);
        } else if (miembro.id > nodo->dato.id) {
            nodo->derecho = insertarRecursivo(nodo->derecho, miembro);
        }
        
        return balancear(nodo);
    }
    
    shared_ptr<Nodo> buscarRecursivo(shared_ptr<Nodo> nodo, int id) const {
        if (!nodo || nodo->dato.id == id) {
            return nodo;
        }
        
        if (id < nodo->dato.id) {
            return buscarRecursivo(nodo->izquierdo, id);
        } else {
            return buscarRecursivo(nodo->derecho, id);
        }
    }
    
    shared_ptr<Nodo> encontrarMinimo(shared_ptr<Nodo> nodo) {
        auto actual = nodo;
        while (actual && actual->izquierdo) {
            actual = actual->izquierdo;
        }
        return actual;
    }
    
    shared_ptr<Nodo> eliminarRecursivo(shared_ptr<Nodo> nodo, int id) {
        if (!nodo) return nodo;
        
        if (id < nodo->dato.id) {
            nodo->izquierdo = eliminarRecursivo(nodo->izquierdo, id);
        } else if (id > nodo->dato.id) {
            nodo->derecho = eliminarRecursivo(nodo->derecho, id);
        } else {
            if (!nodo->izquierdo || !nodo->derecho) {
                auto temp = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;
                nodo.reset();
                return temp;
            } else {
                auto temp = encontrarMinimo(nodo->derecho);
                nodo->dato = temp->dato;
                nodo->derecho = eliminarRecursivo(nodo->derecho, temp->dato.id);
            }
        }
        
        return balancear(nodo);
    }
    
    // ==================== RECORRIDOS ====================
    void inordenRecursivo(shared_ptr<Nodo> nodo, vector<shared_ptr<Nodo>>& resultado) const {
        if (nodo) {
            inordenRecursivo(nodo->izquierdo, resultado);
            resultado.push_back(nodo);
            inordenRecursivo(nodo->derecho, resultado);
        }
    }
    
    void preordenRecursivo(shared_ptr<Nodo> nodo, vector<shared_ptr<Nodo>>& resultado) const {
        if (nodo) {
            resultado.push_back(nodo);
            preordenRecursivo(nodo->izquierdo, resultado);
            preordenRecursivo(nodo->derecho, resultado);
        }
    }
    
    void postordenRecursivo(shared_ptr<Nodo> nodo, vector<shared_ptr<Nodo>>& resultado) const {
        if (nodo) {
            postordenRecursivo(nodo->izquierdo, resultado);
            postordenRecursivo(nodo->derecho, resultado);
            resultado.push_back(nodo);
        }
    }

public:
    ArbolGenealogico() : siguienteID(1000) {
        srand(time(0));
    }
    
    // ==================== MÉTODOS PÚBLICOS ====================
    
    void crearArbolEjemplo() {
        cout << "\n?? CREANDO ÁRBOL DE EJEMPLO..." << endl;
        
        // Crear miembros de ejemplo
        vector<Miembro> miembros = {
            {50, "Fundador Principal", "1900-01-01", "M", 0},
            {30, "Hijo Mayor", "1920-03-20", "M", 1},
            {70, "Hija Menor", "1925-07-12", "F", 1},
            {20, "Nieto A", "1940-05-10", "M", 2},
            {40, "Nieta B", "1942-08-25", "F", 2},
            {60, "Nieto C", "1945-11-30", "M", 2},
            {80, "Nieta D", "1948-12-05", "F", 2},
            {10, "Bisnieto A", "1960-02-14", "M", 3},
            {25, "Bisnieta B", "1962-06-18", "F", 3},
            {35, "Bisnieto C", "1965-09-22", "M", 3}
        };
        
        for (const auto& miembro : miembros) {
            raiz = insertarRecursivo(raiz, miembro);
        }
        
        // Establecer relaciones familiares
        establecerRelacion(50, 30);
        establecerRelacion(50, 70);
        establecerRelacion(30, 20);
        establecerRelacion(30, 40);
        establecerRelacion(70, 60);
        establecerRelacion(70, 80);
        establecerRelacion(20, 10);
        establecerRelacion(20, 25);
        establecerRelacion(40, 35);
        
        cout << "? Árbol de ejemplo creado con 10 miembros" << endl;
    }
    
    void establecerRelacion(int idPadre, int idHijo) {
        auto padre = buscarRecursivo(raiz, idPadre);
        auto hijo = buscarRecursivo(raiz, idHijo);
        
        if (padre && hijo) {
            padre->hijos.push_back(hijo);
            hijo->padre = padre;
        }
    }
    
    void insertarMiembro() {
        cout << "\n?? INSERTAR NUEVO MIEMBRO" << endl;
        
        int id = siguienteID++;
        string nombre, fecha, genero;
        
        cin.ignore();
        cout << "Nombre: "; getline(cin, nombre);
        cout << "Fecha de nacimiento (YYYY-MM-DD): "; cin >> fecha;
        cout << "Género (M/F): "; cin >> genero;
        
        Miembro nuevo(id, nombre, fecha, genero, 0);
        raiz = insertarRecursivo(raiz, nuevo);
        
        cout << "? Miembro insertado con ID: " << id << endl;
    }
    
    void buscarMiembro() {
        cout << "\n?? BUSCAR MIEMBRO" << endl;
        cout << "ID del miembro: ";
        int id;
        cin >> id;
        
        auto resultado = buscarRecursivo(raiz, id);
        if (resultado) {
            cout << "? MIEMBRO ENCONTRADO:" << endl;
            resultado->dato.mostrar();
            cout << endl;
            
            if (resultado->padre) {
                cout << "Padre: " << resultado->padre->dato.nombre << endl;
            }
            
            if (!resultado->hijos.empty()) {
                cout << "Hijos: ";
                for (size_t i = 0; i < resultado->hijos.size(); i++) {
                    cout << resultado->hijos[i]->dato.nombre;
                    if (i < resultado->hijos.size() - 1) cout << ", ";
                }
                cout << endl;
            }
        } else {
            cout << "? Miembro no encontrado" << endl;
        }
    }
    
    void eliminarMiembro() {
        cout << "\n??? ELIMINAR MIEMBRO" << endl;
        cout << "ID del miembro a eliminar: ";
        int id;
        cin >> id;
        
        if (buscarRecursivo(raiz, id)) {
            raiz = eliminarRecursivo(raiz, id);
            cout << "? Miembro eliminado correctamente" << endl;
        } else {
            cout << "? Miembro no encontrado" << endl;
        }
    }
    
    void mostrarRecorrido(const string& tipo) {
        cout << "\n?? RECORRIDO " << tipo << endl;
        cout << string(50, '-') << endl;
        
        vector<shared_ptr<Nodo>> resultado;
        
        if (tipo == "INORDEN") {
            inordenRecursivo(raiz, resultado);
        } else if (tipo == "PREORDEN") {
            preordenRecursivo(raiz, resultado);
        } else if (tipo == "POSTORDEN") {
            postordenRecursivo(raiz, resultado);
        }
        
        for (size_t i = 0; i < resultado.size(); i++) {
            cout << i + 1 << ". ";
            resultado[i]->dato.mostrar();
            cout << endl;
        }
    }
    
    void mostrarPorNiveles() {
        cout << "\n?? RECORRIDO POR NIVELES" << endl;
        cout << string(50, '-') << endl;
        
        if (!raiz) {
            cout << "El árbol está vacío" << endl;
            return;
        }
        
        queue<shared_ptr<Nodo>> cola;
        cola.push(raiz);
        int nivel = 0;
        
        while (!cola.empty()) {
            int tamanoNivel = cola.size();
            cout << "Nivel " << nivel << ": ";
            
            for (int i = 0; i < tamanoNivel; i++) {
                auto nodo = cola.front();
                cola.pop();
                
                cout << nodo->dato.nombre << "(" << nodo->dato.id << ") ";
                
                if (nodo->izquierdo) cola.push(nodo->izquierdo);
                if (nodo->derecho) cola.push(nodo->derecho);
            }
            cout << endl;
            nivel++;
        }
    }
    
    void mostrarEstructura() {
        cout << "\n?? ESTRUCTURA DEL ÁRBOL" << endl;
        cout << string(60, '=') << endl;
        
        if (!raiz) {
            cout << "El árbol está vacío" << endl;
            return;
        }
        
        mostrarArbolRecursivo(raiz, 0);
    }
    
    void mostrarArbolRecursivo(shared_ptr<Nodo> nodo, int espacio) {
        if (!nodo) return;
        
        espacio += 4;
        
        mostrarArbolRecursivo(nodo->derecho, espacio);
        
        cout << endl;
        for (int i = 4; i < espacio; i++) {
            cout << " ";
        }
        cout << nodo->dato.nombre << " [" << nodo->dato.id << "]" << endl;
        
        mostrarArbolRecursivo(nodo->izquierdo, espacio);
    }
    
    void mostrarEstadisticas() {
        cout << "\n?? ESTADÍSTICAS DEL ÁRBOL" << endl;
        cout << string(40, '-') << endl;
        
        if (!raiz) {
            cout << "El árbol está vacío" << endl;
            return;
        }
        
        int total = 0;
        int maxNivel = 0;
        int hombres = 0, mujeres = 0;
        
        vector<shared_ptr<Nodo>> todos;
        inordenRecursivo(raiz, todos);
        
        for (size_t i = 0; i < todos.size(); i++) {
            total++;
            maxNivel = max(maxNivel, todos[i]->dato.nivel);
            if (todos[i]->dato.genero == "M") hombres++;
            else if (todos[i]->dato.genero == "F") mujeres++;
        }
        
        cout << "Total de miembros: " << total << endl;
        cout << "Altura del árbol: " << obtenerAltura(raiz) << endl;
        cout << "Máximo nivel genealógico: " << maxNivel << endl;
        cout << "Hombres: " << hombres << endl;
        cout << "Mujeres: " << mujeres << endl;
        cout << "Balance del árbol: " << obtenerBalance(raiz) << endl;
    }
};

// ==================== MENÚ PRINCIPAL ====================
void mostrarMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "     SISTEMA DE ÁRBOL GENEALÓGICO" << endl;
    cout << string(50, '=') << endl;
    cout << "1. Crear árbol de ejemplo (10 miembros)" << endl;
    cout << "2. Insertar nuevo miembro" << endl;
    cout << "3. Buscar miembro" << endl;
    cout << "4. Eliminar miembro" << endl;
    cout << "5. Mostrar recorrido INORDEN" << endl;
    cout << "6. Mostrar recorrido PREORDEN" << endl;
    cout << "7. Mostrar recorrido POSTORDEN" << endl;
    cout << "8. Mostrar por niveles" << endl;
    cout << "9. Mostrar estructura del árbol" << endl;
    cout << "10. Mostrar estadísticas" << endl;
    cout << "0. Salir" << endl;
    cout << string(50, '-') << endl;
    cout << "Seleccione una opción: ";
}

int main() {
    ArbolGenealogico arbol;
    int opcion;
    
    cout << "?? SISTEMA DE ÁRBOL GENEALÓGICO INICIADO" << endl;
    
    do {
        mostrarMenu();
        cin >> opcion;
        
        switch(opcion) {
            case 1:
                arbol.crearArbolEjemplo();
                break;
            case 2:
                arbol.insertarMiembro();
                break;
            case 3:
                arbol.buscarMiembro();
                break;
            case 4:
                arbol.eliminarMiembro();
                break;
            case 5:
                arbol.mostrarRecorrido("INORDEN");
                break;
            case 6:
                arbol.mostrarRecorrido("PREORDEN");
                break;
            case 7:
                arbol.mostrarRecorrido("POSTORDEN");
                break;
            case 8:
                arbol.mostrarPorNiveles();
                break;
            case 9:
                arbol.mostrarEstructura();
                break;
            case 10:
                arbol.mostrarEstadisticas();
                break;
            case 0:
                cout << "?? ¡Hasta pronto!" << endl;
                break;
            default:
                cout << "? Opción no válida" << endl;
        }
        
        if (opcion != 0) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
        
    } while (opcion != 0);
    
    return 0;
}
