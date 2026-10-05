#include <iostream>
#include <stack>

using namespace std;

int main(){
	// crear una pila
	stack <int> pila1;
	
	//agregar elementos en la pila
	pila1.push(150);
	pila1.push(250);
	pila1.push(-50);
	
	cout<< "El elemento que esta en la cima de la pila1 es" << pila1.top()<<endl;
	
	//Eliminar elemento de la pila 
	pila1.pop();
	cout<<"Ahora el elemento en la cima de la pila1 es " <<pila1.top()<<endl;
	
	//Mostrar todos los elementos de la pila
	cout<<"A continuacion todos los elementos de la pila1: "<<endl; 
	
	while( !pila1.empty()){
		cout<< pila1.top() << endl;
		pila1.pop();
	}
	
	
	return 0;
}