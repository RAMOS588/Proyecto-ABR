#include <iostream>
#include <queue>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

// ============================================
//   ESTRUCTURAS PRINCIPALES
// ============================================

// Representa a un miembro del árbol genealógico
struct Miembro {
    int id;
    string nombre;
    string fecha;

    Miembro(int i=0, string n="", string f="") : id(i), nombre(n), fecha(f) {}
};

// Nodo del árbol AVL, contiene un miembro y punteros
struct Nodo {
    Miembro dato;
    Nodo* izq;
    Nodo* der;
    int altura;

    Nodo(Miembro m) : dato(m), izq(nullptr), der(nullptr), altura(1) {}
};


// ============================================
//      CLASE PRINCIPAL DEL ÁRBOL AVL
// ============================================
class ArbolGenealogico {
	
private:
    Nodo* raiz;

    // Devuelve la altura de un nodo
    int altura(Nodo* n) {
        return n ? n->altura : 0;
    }

    // Calcula el factor de balance para el nodo
    int balance(Nodo* n) {
        return n ? altura(n->izq) - altura(n->der) : 0;
    }

    // Rotación simple a la derecha (caso LL)
    Nodo* rotDer(Nodo* y) {
        Nodo* x = y->izq;
        Nodo* T2 = x->der;

        x->der = y;
        y->izq = T2;

        // Actualización de alturas después de la rotación
        y->altura = max(altura(y->izq), altura(y->der)) + 1;
        x->altura = max(altura(x->izq), altura(x->der)) + 1;

        return x;
    }

    // Rotación simple a la izquierda (caso RR)
    Nodo* rotIzq(Nodo* x) {
        Nodo* y = x->der;
        Nodo* T2 = y->izq;

        y->izq = x;
        x->der = T2;

        x->altura = max(altura(x->izq), altura(x->der)) + 1;
        y->altura = max(altura(y->izq), altura(y->der)) + 1;

        return y;
    }

    // Inserta un nodo manteniendo las reglas del AVL
    Nodo* insertar(Nodo* nodo, Miembro m) {
        if (!nodo) return new Nodo(m);

        // Inserción normal como ABB
        if (m.id < nodo->dato.id)
            nodo->izq = insertar(nodo->izq, m);
        else if (m.id > nodo->dato.id)
            nodo->der = insertar(nodo->der, m);
        else
            return nodo; // ID duplicado, no se inserta

        // Actualiza altura y balancea el nodo
        nodo->altura = 1 + max(altura(nodo->izq), altura(nodo->der));
        int b = balance(nodo);

        // Cuatro casos del AVL
        if (b > 1 && m.id < nodo->izq->dato.id) return rotDer(nodo);
        if (b < -1 && m.id > nodo->der->dato.id) return rotIzq(nodo);
        if (b > 1 && m.id > nodo->izq->dato.id) {
            nodo->izq = rotIzq(nodo->izq);
            return rotDer(nodo);
        }
        if (b < -1 && m.id < nodo->der->dato.id) {
            nodo->der = rotDer(nodo->der);
            return rotIzq(nodo);
        }

        return nodo;
    }

    // Busca un nodo por ID
    Nodo* buscarNodo(Nodo* nodo, int id) {
        if (!nodo) return nullptr;
        if (id == nodo->dato.id) return nodo;
        if (id < nodo->dato.id) return buscarNodo(nodo->izq, id);
        return buscarNodo(nodo->der, id);
    }

    // Encuentra el nodo mínimo (para eliminar)
    Nodo* minimo(Nodo* nodo) {
        while (nodo->izq) nodo = nodo->izq;
        return nodo;
    }

    // Elimina un nodo y rebalancea si es necesario
    Nodo* eliminar(Nodo* nodo, int id) {
        if (!nodo) return nodo;

        if (id < nodo->dato.id)
            nodo->izq = eliminar(nodo->izq, id);
        else if (id > nodo->dato.id)
            nodo->der = eliminar(nodo->der, id);
        else {

            // Caso: 0 o 1 hijo
            if (!nodo->izq || !nodo->der) {
                Nodo* temp = nodo->izq ? nodo->izq : nodo->der;

                if (!temp) {
                    temp = nodo;
                    nodo = nullptr;
                } else {
                    *nodo = *temp;
                }
                delete temp;
            }
            // Caso: 2 hijos → reemplazar por el sucesor
            else {
                Nodo* temp = minimo(nodo->der);
                nodo->dato = temp->dato;
                nodo->der = eliminar(nodo->der, temp->dato.id);
            }
        }

        if (!nodo) return nodo;

        nodo->altura = 1 + max(altura(nodo->izq), altura(nodo->der));
        int b = balance(nodo);

        // Casos de rebalanceo
        if (b > 1 && balance(nodo->izq) >= 0) return rotDer(nodo);
        if (b > 1 && balance(nodo->izq) < 0) {
            nodo->izq = rotIzq(nodo->izq);
            return rotDer(nodo);
        }
        if (b < -1 && balance(nodo->der) <= 0) return rotIzq(nodo);
        if (b < -1 && balance(nodo->der) > 0) {
            nodo->der = rotDer(nodo->der);
            return rotIzq(nodo);
        }

        return nodo;
    }

    // ================================
    // RECORRIDOS
    // ================================
    void inorden(Nodo* n) {
        if (!n) return;
        inorden(n->izq);
        cout << n->dato.nombre << " (" << n->dato.id << ")\n";
        inorden(n->der);
    }

    void preorden(Nodo* n) {
        if (!n) return;
        cout << n->dato.nombre << " (" << n->dato.id << ")\n";
        preorden(n->izq);
        preorden(n->der);
    }

    void postorden(Nodo* n) {
        if (!n) return;
        postorden(n->izq);
        postorden(n->der);
        cout << n->dato.nombre << " (" << n->dato.id << ")\n";
    }

    // ================================
    // MOSTRAR POR NIVELES (BFS)
    // ================================
    void niveles(Nodo* r) {
        if (!r) return;
        queue<Nodo*> q;
        q.push(r);

        while (!q.empty()) {
            int tam = q.size();
            while (tam--) {
                Nodo* act = q.front(); q.pop();
                cout << act->dato.nombre << "(" << act->dato.id << ") ";
                if (act->izq) q.push(act->izq);
                if (act->der) q.push(act->der);
            }
            cout << "\n";
        }
    }

    // ================================
    // ESQUEMA PIRAMIDAL DEL ÁRBOL
    // ================================
    void imprimirPiramide(Nodo* root) {
        if (!root) return;

        vector<vector<string>> niveles;
        queue<Nodo*> q;
        q.push(root);

        // Construye una matriz por niveles
        while (!q.empty()) {
            int tam = q.size();
            vector<string> nivel;

            while (tam--) {
                Nodo* act = q.front(); q.pop();

                if (act)
                    nivel.push_back(act->dato.nombre + "(" + to_string(act->dato.id) + ")");
                else
                    nivel.push_back(" ");

                if (act) {
                    q.push(act->izq);
                    q.push(act->der);
                } else {
                    q.push(nullptr);
                    q.push(nullptr);
                }
            }

            bool todosNull = true;
            for (auto &s : nivel)
                if (s != " ") todosNull = false;

            if (todosNull) break;

            niveles.push_back(nivel);
        }

        int ancho = 60;
        for (int i = 0; i < niveles.size(); i++) {
            int esp = ancho / (i + 1);
            for (auto &s : niveles[i]) {
                cout << setw(esp) << s;
            }
            cout << "\n\n";
        }
    }

public:
    ArbolGenealogico() : raiz(nullptr) {}

    // Inserta un nuevo miembro en el árbol AVL
    void insertarMiembro(int id, string nom, string fec) {
        raiz = insertar(raiz, Miembro(id, nom, fec));
    }

    // Elimina un miembro por ID
    void eliminarMiembro(int id) {
        raiz = eliminar(raiz, id);
    }

    // Busca un miembro y muestra información
    void buscarMiembro(int id) {
        Nodo* r = buscarNodo(raiz, id);
        if (r) cout << "Miembro encontrado: " << r->dato.nombre << endl;
        else cout << "El ID no existe en el arbol.\n";
    }

    // Funciones públicas de impresión
    void mostrarInorden() { inorden(raiz); }
    void mostrarPreorden() { preorden(raiz); }
    void mostrarPostorden() { postorden(raiz); }
    void verNiveles() { niveles(raiz); }
    void verPiramide() { imprimirPiramide(raiz); }

    // Árbol de ejemplo
    void cargarAnkarai() {
        insertarMiembro(50, "Arkan", "1500");
        insertarMiembro(30, "Dario", "1528");
        insertarMiembro(70, "Marco", "1530");
        insertarMiembro(20, "Sara", "1550");
        insertarMiembro(40, "Talia", "1552");
        insertarMiembro(60, "Leo", "1575");
        insertarMiembro(80, "Mira", "1578");
        insertarMiembro(10, "Ana", "1580");
        insertarMiembro(25, "Elias", "1583");
        insertarMiembro(35, "Raul", "1585");
    }
};


// ============================================
//          MENÚ PRINCIPAL INTERACTIVO
// ============================================
int main() {
	setlocale(LC_ALL, "Spanish");
    ArbolGenealogico A;
    int op;

    do {
        cout << "\n================ MENU DEL SISTEMA ================\n";
        cout << "1. Insertar nuevo miembro\n";
        cout << "2. Buscar miembro por ID\n";
        cout << "3. Eliminar miembro del arbol\n";
        cout << "4. Mostrar recorrido INORDEN\n";
        cout << "5. Mostrar recorrido PREORDEN\n";
        cout << "6. Mostrar recorrido POSTORDEN\n";
        cout << "7. Mostrar arbol por niveles\n";
        cout << "8. Mostrar esquema del arbol (vista piramidal)\n";
        cout << "9. Cargar Arbol de ejemplo: Civilizacion ANKARAI\n";
        cout << "0. Salir del programa\n";
        cout << "Seleccione una opcion: ";
        cin >> op;

        if (op == 1) {
            cout << "\n--- INSERTANDO NUEVO MIEMBRO ---\n";
            int id; string nom, fec;
            cout << "ID numerico: "; cin >> id;
            cout << "Nombre: "; cin >> nom;
            cout << "Fecha de nacimiento: "; cin >> fec;
            A.insertarMiembro(id, nom, fec);
            cout << "Miembro insertado correctamente.\n";
        }

        else if (op == 2) {
            cout << "\n--- BUSCAR MIEMBRO ---\n";
            int id;
            cout << "Ingrese ID a buscar: ";
            cin >> id;
            A.buscarMiembro(id);
        }

        else if (op == 3) {
            cout << "\n--- ELIMINACION DE MIEMBRO ---\n";
            int id;
            cout << "Ingrese ID a eliminar: ";
            cin >> id;
            A.eliminarMiembro(id);
            cout << "Si el ID existia, ha sido eliminado.\n";
        }

        else if (op == 4) {
            cout << "\n--- RECORRIDO INORDEN (menor a mayor) ---\n";
            A.mostrarInorden();
        }

        else if (op == 5) {
            cout << "\n--- RECORRIDO PREORDEN  ---\n";
            A.mostrarPreorden();
        }

        else if (op == 6) {
            cout << "\n--- RECORRIDO POSTORDEN ---\n";
            A.mostrarPostorden();
        }

        else if (op == 7) {
            cout << "\n--- ARBOL POR NIVELES (generaciones) ---\n";
            A.verNiveles();
        }

        else if (op == 8) {
            cout << "\n--- ESQUEMA PIRAMIDAL DEL ARBOL ---\n";
            A.verPiramide();
        }

        else if (op == 9) {
            cout << "\nCargando Arbol genealagico de la civilizacion ANKARAI...\n";
            A.cargarAnkarai();
            cout << "Arbol cargado exitosamente.\n";
        }

    } while (op != 0);

    cout << "\nPrograma finalizado. �Hasta luego!\n";
    return 0;
}
