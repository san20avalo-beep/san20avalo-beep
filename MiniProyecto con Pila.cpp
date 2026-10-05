#include <iostream>
#include <string>
#include <cstdlib> 
#include <ctime>   
#include <stack>    // Incluimos la librería para usar pilas

using namespace std;

// Estructura para almacenar los datos de cada cliente en la pila
struct Cliente {
    string identidad;
    int tipo;
};

int main() {
    srand(time(0)); 

    // Creamos la pila de clientes
    stack<Cliente> pilaClientes;
    
    char continuar;

    cout << "  ____    _    _   _  ____  ___  " << endl;
    cout << " | __ )  / \\  | \\ | |/ ___|/ _ \\ " << endl;
    cout << " |  _ \\ / _ \\ |  \\| | |   | | | |" << endl;
    cout << " | |_) / ___ \\| |\\  | |___| |_| |" << endl;
    cout << " |____/_/   \\_\\_| \\_|\\____|\\___/ " << endl;
    cout << "    S I S T E M A  B A N C A R I O" << endl;
    cout << "========================================" << endl;
    cout << "          CONTROL DE TURNOS " << endl;
    cout << "========================================" << endl;
    cout << endl;
    
    // FASE 1: REGISTRO DE CLIENTES (PUSH)
    do {
        Cliente nuevoCliente;
        bool identidadValida = false;  

        // Validación de identidad
        do {
            cout << "Ingrese su numero de identidad (con guiones): ";
            cin >> nuevoCliente.identidad;    
            
            if (nuevoCliente.identidad.length() == 15 && nuevoCliente.identidad[4] == '-' && nuevoCliente.identidad[9] == '-') {
                identidadValida = true;  
            } else {
                cout << "\n[ERROR] El numero de identidad ingresado es incorrecto." << endl;
                cout << "Asegurese de incluir los guiones y que tenga el formato valido.\n" << endl;
            }
        } while (!identidadValida); 
        
        // Selección de transacción
        cout << "\nSeleccione tipo de transaccion:" << endl;
        cout << "1. Caja" << endl;
        cout << "2. Caja Empresarial" << endl;
        cout << "3. Servicio al Cliente" << endl;
        cout << "Opcion a elegir: ";
        cin >> nuevoCliente.tipo;   
        
        // Insertar el cliente en la parte superior de la pila
        pilaClientes.push(nuevoCliente);
        cout << "\n[OK] Cliente registrado exitosamente en la pila.\n" << endl;

        cout << "Desea registrar otro cliente? (s/n): ";
        cin >> continuar;
        cout << "----------------------------------------" << endl;

    } while (continuar == 's' || continuar == 'S');

    // FASE 2: ATENCIÓN DE CLIENTES (POP)
    cout << endl;
    cout << "========================================" << endl;
    cout << "   PROCESANDO Y ATENDIENDO TURNOS" << endl;
    cout << "========================================" << endl;
    
    if (pilaClientes.empty()) {
        cout << "No hay clientes en la fila." << endl;
    }

    // Mientras la pila no esté vacía, seguimos atendiendo
    while (!pilaClientes.empty()) {
    	
        // Obtenemos el cliente que está arriba (el último que entró)
        Cliente clienteActual = pilaClientes.top(); 
        
        cout << endl;
        cout << "================================" << endl;
        cout << "      SU BOLETA DE TURNO" << endl;
        cout << "================================" << endl;
        cout << "Identidad: " << clienteActual.identidad << endl;  
        
        int cubiculo;
        if (clienteActual.tipo == 1) {
            cout << "Tipo: Caja" << endl;
            cout << "Dirijase al cubiculo: ";
            cubiculo = 1 + rand() % 8;  
            cout << cubiculo << endl; 
        }
        else if (clienteActual.tipo == 2) {
            cout << "Tipo: Caja Empresarial" << endl;
            cout << "Dirijase al cubiculo: ";
            cubiculo = 9 + rand() % 8;  
            cout << cubiculo << endl;
        }
        else if (clienteActual.tipo == 3) {
            cout << "Tipo: Servicio al Cliente" << endl;
            cout << "Dirijase al cubiculo: ";
            cubiculo = 17 + rand() % 8; 
            cout << cubiculo << endl;
        }
        else {
            cout << "Opcion invalida (No se puede asignar cubiculo)" << endl;
        }
        cout << "================================" << endl;

        // Sacamos al cliente de la pila para pasar al siguiente
        pilaClientes.pop(); 
    }
    
    cout << "\n[FIN] Todos los clientes de la pila han sido atendidos." << endl;
    return 0;
}