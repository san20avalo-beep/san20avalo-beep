#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>
#include <sstream>

using namespace std;

// ==========================================
// 1. ESTRUCTURAS DE DATOS BASE
// ==========================================

// Tipos de lugares
enum TipoLugar { CIUDAD, CUEVA, BOSQUE, MONTANA, CASTILLO };

string tipoAString(TipoLugar t) {
    switch (t) {
        case CIUDAD: return "Ciudad";
        case CUEVA: return "Cueva";
        case BOSQUE: return "Bosque";
        case MONTANA: return "Montana";
        case CASTILLO: return "Castillo";
        default: return "Desconocido";
    }
}

struct Lugar {
    int id;
    string nombre;
    TipoLugar tipo;
};

// Arista con pesos múltiples
struct Camino {
    int destinoId;
    int distancia;
    int nivelPeligro;
    int energiaNecesaria;
    int tiempoRecorrido;
};

// Lista Doble para Historial
struct NodoHistorial {
    int lugarId;
    NodoHistorial* anterior;
    NodoHistorial* siguiente;
    NodoHistorial(int id) : lugarId(id), anterior(nullptr), siguiente(nullptr) {}
};

class HistorialDoble {
public:
    NodoHistorial* cabeza;
    NodoHistorial* cola;
    HistorialDoble() : cabeza(nullptr), cola(nullptr) {}

    void agregar(int id) {
        NodoHistorial* nuevo = new NodoHistorial(id);
        if (!cabeza) {
            cabeza = cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            nuevo->anterior = cola;
            cola = nuevo;
        }
    }

    int deshacer() {
        if (!cola || !cola->anterior) return -1;
        NodoHistorial* temp = cola;
        cola = cola->anterior;
        cola->siguiente = nullptr;
        int idRegreso = cola->lugarId;
        delete temp;
        return idRegreso;
    }
};

// Lista Circular de Turnos
struct NodoTurno {
    string entidad; // Jugador o Enemigo
    NodoTurno* siguiente;
    NodoTurno(string e) : entidad(e), siguiente(nullptr) {}
};

class ListaCircularTurnos {
public:
    NodoTurno* actual;
    ListaCircularTurnos() : actual(nullptr) {}

    void agregar(string e) {
        NodoTurno* nuevo = new NodoTurno(e);
        if (!actual) {
            actual = nuevo;
            actual->siguiente = actual;
        } else {
            nuevo->siguiente = actual->siguiente;
            actual->siguiente = nuevo;
        }
    }

    string siguienteTurno() {
        if (actual) {
            actual = actual->siguiente;
            return actual->entidad;
        }
        return "";
    }
};

// ==========================================
// 2. MOTOR DE GRAFO Y JUEGO
// ==========================================

class JuegoMapa {
public:
    vector<Lugar> lugares;
    vector<vector<Camino>> adyacencia;
    vector<string> enemigos;
    vector<string> inventario;
    HistorialDoble historial;
    ListaCircularTurnos turnos;

    int ubicacionActual;
    int energiaJugador;

    JuegoMapa() : ubicacionActual(-1), energiaJugador(100) {
        // Inicializar Turnos
        turnos.agregar("Jugador");
        turnos.agregar("Enemigo Goblin");
        turnos.agregar("Enemigo Dragon");

        // Inicializar Inventario de Objetos
        inventario.push_back("Espada de Hierro");
        inventario.push_back("Pocion de Energia (+30)");
        inventario.push_back("Mapa Antiguo");

        // Inicializar Enemigos del Juego
        enemigos.push_back("Orco Guerrero");
        enemigos.push_back("Espectro del Bosque");
        enemigos.push_back("Dragon de Fuego");
    }

    void agregarLugar(string nombre, TipoLugar tipo) {
        int id = lugares.size();
        lugares.push_back({id, nombre, tipo});
        adyacencia.push_back(vector<Camino>());
        if (ubicacionActual == -1) {
            ubicacionActual = id;
            historial.agregar(id);
        }
    }

    void conectarLugares(int u, int v, int dist, int peligro, int energia, int tiempo) {
        if (u >= 0 && u < lugares.size() && v >= 0 && v < lugares.size()) {
            adyacencia[u].push_back({v, dist, peligro, energia, tiempo});
            adyacencia[v].push_back({u, dist, peligro, energia, tiempo}); // Grafo no dirigido
        }
    }

    // 4. Mover al jugador
    bool moverJugador(int destinoId, string& msgLog) {
        for (const auto& camino : adyacencia[ubicacionActual]) {
            if (camino.destinoId == destinoId) {
                if (energiaJugador >= camino.energiaNecesaria) {
                    energiaJugador -= camino.energiaNecesaria;
                    ubicacionActual = destinoId;
                    historial.agregar(destinoId);
                    msgLog = "Movimiento exitoso a " + lugares[destinoId].nombre + ". Energia consumida: " + to_string(camino.energiaNecesaria);
                    return true;
                } else {
                    msgLog = "Energia insuficiente. Requieres " + to_string(camino.energiaNecesaria) + " pero tienes " + to_string(energiaJugador);
                    return false;
                }
            }
        }
        msgLog = "No existe un camino directo hacia ese lugar.";
        return false;
    }

    // 8. Regresar a ubicación anterior
    bool regresarAnterior(string& msgLog) {
        int anteriorId = historial.deshacer();
        if (anteriorId != -1) {
            ubicacionActual = anteriorId;
            msgLog = "Has regresado a: " + lugares[ubicacionActual].nombre;
            return true;
        }
        msgLog = "No hay ubicaciones anteriores en el historial.";
        return false;
    }

    // 5. BFS para lugares alcanzables dentro de un rango
    string bfsAlcanzables(int inicioId) {
        vector<bool> visitado(lugares.size(), false);
        queue<int> q;
        stringstream ss;

        q.push(inicioId);
        visitado[inicioId] = true;

        ss << "Lugares alcanzables mediante BFS desde " << lugares[inicioId].nombre << ":\n";

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            if (u != inicioId) {
                ss << "- " << lugares[u].nombre << " (" << tipoAString(lugares[u].tipo) << ")\n";
            }

            for (const auto& vecino : adyacencia[u]) {
                if (!visitado[vecino.destinoId]) {
                    visitado[vecino.destinoId] = true;
                    q.push(vecino.destinoId);
                }
            }
        }
        return ss.str();
    }

    // 10. Detectar zonas inaccesibles (DFS)
    string detectarInaccesibles() {
        vector<bool> visitado(lugares.size(), false);
        stack<int> s;

        if (!lugares.empty()) {
            s.push(0);
            visitado[0] = true;
            while (!s.empty()) {
                int u = s.top();
                s.pop();
                for (const auto& vecino : adyacencia[u]) {
                    if (!visitado[vecino.destinoId]) {
                        visitado[vecino.destinoId] = true;
                        s.push(vecino.destinoId);
                    }
                }
            }
        }

        stringstream ss;
        ss << "Zonas Inaccesibles (Aisladas del inicio):\n";
        bool hayInaccesibles = false;
        for (size_t i = 0; i < lugares.size(); ++i) {
            if (!visitado[i]) {
                ss << "- " << lugares[i].nombre << "\n";
                hayInaccesibles = true;
            }
        }
        if (!hayInaccesibles) ss << "Ninguna. Todas las zonas son conexas.\n";
        return ss.str();
    }

    // 7. Dijkstra: Camino más corto (Distancia)
    // 6. Dijkstra: Camino más seguro (Nivel de Peligro)
    // Reto Avanzado: Filtro de Energía Máxima Limite
    string dijkstraRuta(int origen, int destino, bool porPeligro, int maxEnergia) {
        int n = lugares.size();
        vector<int> dist(n, INT_MAX);
        vector<int> energiaAcumulada(n, INT_MAX);
        vector<int> padre(n, -1);

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[origen] = 0;
        energiaAcumulada[origen] = 0;
        pq.push({0, origen});

        while (!pq.empty()) {
            int u = pq.top().second;
            int d = pq.top().first;
            pq.pop();

            if (d > dist[u]) continue;
            if (u == destino) break;

            for (const auto& camino : adyacencia[u]) {
                int v = camino.destinoId;
                int peso = porPeligro ? camino.nivelPeligro : camino.distancia;
                int nuevaEnergia = energiaAcumulada[u] + camino.energiaNecesaria;

                // Restricción del Reto Avanzado: No superar el límite de energía
                if (nuevaEnergia <= maxEnergia) {
                    if (dist[u] + peso < dist[v]) {
                        dist[v] = dist[u] + peso;
                        energiaAcumulada[v] = nuevaEnergia;
                        padre[v] = u;
                        pq.push({dist[v], v});
                    }
                }
            }
        }

        stringstream ss;
        if (dist[destino] == INT_MAX) {
            ss << "No existe una ruta valida a " << lugares[destino].nombre 
               << " respetando el limite de energia de (" << maxEnergia << ").\n";
            return ss.str();
        }

        // Reconstrucción del camino encontrado
        vector<int> ruta;
        for (int v = destino; v != -1; v = padre[v]) ruta.push_back(v);
        reverse(ruta.begin(), ruta.end());

        ss << "Ruta " << (porPeligro ? "Mas Segura" : "Mas Corta") << " a " << lugares[destino].nombre << ":\n";
        for (size_t i = 0; i < ruta.size(); ++i) {
            ss << lugares[ruta[i]].nombre << (i + 1 < ruta.size() ? " -> " : "");
        }
        ss << "\nCosto acumulado: " << dist[destino];
        ss << " | Energia total requerida: " << energiaAcumulada[destino] << "/" << maxEnergia << "\n";

        return ss.str();
    }
};

// INSTANCIA GLOBAL DEL JUEGO
JuegoMapa juego;

// ==========================================
// 3. INTERFAZ WIN32 CONTROLES ID
// ==========================================

#define ID_LIST_CONEXIONES 101
#define ID_BTN_MOVER       102
#define ID_BTN_REGRESAR    103
#define ID_BTN_BFS         104
#define ID_BTN_CORTO       105
#define ID_BTN_SEGURO      106
#define ID_BTN_INACCESIBLE 107
#define ID_BTN_TURNO       108
#define ID_TXT_LOG         109

HWND hListConexiones, hTxtEstado, hTxtLog;

void CargarDatosIniciales() {
    // 1. Crear lugares
    juego.agregarLugar("Capital Valoria", CIUDAD);
    juego.agregarLugar("Bosque Sombrio", BOSQUE);
    juego.agregarLugar("Cueva del Dragón", CUEVA);
    juego.agregarLugar("Montanas de la Niebla", MONTANA);
    juego.agregarLugar("Castillo Oscuro", CASTILLO);
    juego.agregarLugar("Isla Desierta", CIUDAD); // Zona inaccesible para prueba

    // 2 y 3. Conectar lugares y asignar dificultad a los caminos
    // (u, v, distancia, peligro, energia, tiempo)
    juego.conectarLugares(0, 1, 10, 2, 15, 5); // Valoria <-> Bosque
    juego.conectarLugares(1, 2, 25, 8, 30, 12); // Bosque <-> Cueva
    juego.conectarLugares(0, 3, 15, 4, 20, 8); // Valoria <-> Montañas
    juego.conectarLugares(3, 4, 30, 9, 45, 15); // Montañas <-> Castillo
    juego.conectarLugares(1, 4, 50, 6, 40, 20); // Bosque <-> Castillo
}

void ActualizarInterfaz(HWND hwnd) {
    // Actualizar Panel de Estado del Jugador
    stringstream ss;
    ss << "Ubicacion Actual: " << juego.lugares[juego.ubicacionActual].nombre 
       << " (" << tipoAString(juego.lugares[juego.ubicacionActual].tipo) << ")"
       << "  |  Energia Disponible: " << juego.energiaJugador << "/100";
    SetWindowText(hTxtEstado, ss.str().c_str());

    // Actualizar ListBox de Conexiones
    SendMessage(hListConexiones, LB_RESETCONTENT, 0, 0);
    for (const auto& camino : juego.adyacencia[juego.ubicacionActual]) {
        string desc = juego.lugares[camino.destinoId].nombre + 
                      " [Dist: " + to_string(camino.distancia) + 
                      " | Peligro: " + to_string(camino.nivelPeligro) + 
                      " | Energia: " + to_string(camino.energiaNecesaria) + "]";
        int index = SendMessage(hListConexiones, LB_ADDSTRING, 0, (LPARAM)desc.c_str());
        SendMessage(hListConexiones, LB_SETITEMDATA, index, camino.destinoId);
    }
}

void AppendLog(string texto) {
    int len = GetWindowTextLength(hTxtLog);
    SendMessage(hTxtLog, EM_SETSEL, len, len);
    SendMessage(hTxtLog, EM_REPLACESEL, FALSE, (LPARAM)(texto + "\r\n---------------------------------------\r\n").c_str());
}

// ==========================================
// 4. BUCLE DE VENTANA WIN32
// ==========================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_CREATE: {
            CargarDatosIniciales();

            // Titulo de estado
            hTxtEstado = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_LEFT,
                20, 15, 600, 25, hwnd, NULL, NULL, NULL);

            // Lista de Adyacencia / Lugares Alcanzables
            CreateWindow("STATIC", "Conexiones Directas / Caminos:", WS_CHILD | WS_VISIBLE,
                20, 45, 250, 20, hwnd, NULL, NULL, NULL);
            hListConexiones = CreateWindow("LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
                20, 70, 300, 140, hwnd, (HMENU)ID_LIST_CONEXIONES, NULL, NULL);

            // Botones de Accion
            CreateWindow("BUTTON", "Mover a Seleccion", WS_CHILD | WS_VISIBLE,
                340, 70, 160, 30, hwnd, (HMENU)ID_BTN_MOVER, NULL, NULL);
            CreateWindow("BUTTON", "Regresar (Historial)", WS_CHILD | WS_VISIBLE,
                340, 110, 160, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
            CreateWindow("BUTTON", "Lugares BFS", WS_CHILD | WS_VISIBLE,
                340, 150, 160, 30, hwnd, (HMENU)ID_BTN_BFS, NULL, NULL);

            CreateWindow("BUTTON", "Ruta Mas Corta", WS_CHILD | WS_VISIBLE,
                515, 70, 160, 30, hwnd, (HMENU)ID_BTN_CORTO, NULL, NULL);
            CreateWindow("BUTTON", "Ruta Mas Segura", WS_CHILD | WS_VISIBLE,
                515, 110, 160, 30, hwnd, (HMENU)ID_BTN_SEGURO, NULL, NULL);
            CreateWindow("BUTTON", "Zonas Inaccesibles", WS_CHILD | WS_VISIBLE,
                515, 150, 160, 30, hwnd, (HMENU)ID_BTN_INACCESIBLE, NULL, NULL);
            CreateWindow("BUTTON", "Siguiente Turno", WS_CHILD | WS_VISIBLE,
                340, 190, 335, 30, hwnd, (HMENU)ID_BTN_TURNO, NULL, NULL);

            // Cuadro de Log y Consola Visual
            CreateWindow("STATIC", "Consola de Resultados y Algoritmos:", WS_CHILD | WS_VISIBLE,
                20, 225, 300, 20, hwnd, NULL, NULL, NULL);
            hTxtLog = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY,
                20, 245, 655, 200, hwnd, (HMENU)ID_TXT_LOG, NULL, NULL);

            ActualizarInterfaz(hwnd);
            AppendLog("Bienvenido al Motor de Navegacion de Videojuego.\nInicias en la Capital Valoria.");
            break;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case ID_BTN_MOVER: {
                    int sel = SendMessage(hListConexiones, LB_GETCURSEL, 0, 0);
                    if (sel != LB_ERR) {
                        int destinoId = SendMessage(hListConexiones, LB_GETITEMDATA, sel, 0);
                        string msg;
                        if (juego.moverJugador(destinoId, msg)) {
                            ActualizarInterfaz(hwnd);
                        }
                        AppendLog(msg);
                    } else {
                        MessageBox(hwnd, "Selecciona una ubicacion de la lista.", "Aviso", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                }

                case ID_BTN_REGRESAR: {
                    string msg;
                    if (juego.regresarAnterior(msg)) {
                        ActualizarInterfaz(hwnd);
                    }
                    AppendLog(msg);
                    break;
                }

                case ID_BTN_BFS: {
                    string res = juego.bfsAlcanzables(juego.ubicacionActual);
                    AppendLog(res);
                    break;
                }

                case ID_BTN_CORTO: {
                    // Calcula ruta al Castillo (ID 4) considerando limite de energia
                    string res = juego.dijkstraRuta(juego.ubicacionActual, 4, false, juego.energiaJugador);
                    AppendLog(res);
                    break;
                }

                case ID_BTN_SEGURO: {
                    // Calcula ruta al Castillo (ID 4) guiado por peligro
                    string res = juego.dijkstraRuta(juego.ubicacionActual, 4, true, juego.energiaJugador);
                    AppendLog(res);
                    break;
                }

                case ID_BTN_INACCESIBLE: {
                    string res = juego.detectarInaccesibles();
                    AppendLog(res);
                    break;
                }

                case ID_BTN_TURNO: {
                    string ent = juego.turnos.siguienteTurno();
                    AppendLog("Es el turno de: " + ent);
                    break;
                }
            }
            break;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

// ==========================================
// 5. PUNTO DE ENTRADA WINMAIN
// ==========================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Error al registrar la ventana!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "WindowClass",
        "Motor de Navegacion de Videojuego - Win32 Grafo",
        WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        710, 500,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Error al crear la ventana!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}