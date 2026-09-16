#include "../Comportamientos_Jugador/auxiliar.hpp"
#include <iostream>
#include "motorlib/util.h"

Action ComportamientoAuxiliar::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoAuxiliarNivel_0 (sensores);
		break;
	case 1:
		accion = ComportamientoAuxiliarNivel_1 (sensores);
		break;
	case 2:
		accion = ComportamientoAuxiliarNivel_2 (sensores);
		break;
	case 3:
		accion = ComportamientoAuxiliarNivel_3 (sensores);
		break;
	case 4:
		accion = ComportamientoAuxiliarNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoAuxiliar::interact(Action accion, int valor)
{
	return 0;
}


/*
******************************************************
			FUNCIONES PARA EL NIVEL 0
******************************************************
*/


// Función auxiliar para calcular la siguiente casilla en función de la orientación
pair<int, int> NextCasillaA_N01(const pair<int, int> &st, Orientacion rumbo){

	pair<int, int> siguiente = st;

	switch(rumbo){

		case norte:
			siguiente.first--;
			break;
		case noreste:
			siguiente.first--;
			siguiente.second++;
			break;
		case este:
			siguiente.second++;
			break;
		case sureste:
			siguiente.first++;
			siguiente.second++;
			break;
		case sur:
			siguiente.first++;
			break;
		case suroeste:
			siguiente.first++;
			siguiente.second--;
			break;
		case oeste:
			siguiente.second--;
			break;
		case noroeste:
			siguiente.first--;
			siguiente.second--;
			break;
		default:
			break;

	}

	return siguiente;

}


// Función para detectar algún agente en el cono de visión
bool agenteVisibleA(Sensores sensores) {

	for(int i=1; i<=8; i++){
		if(sensores.agentes[i] != '_'){
			return true;
		}
	}

	return false;
}


// Método para obtener la última vez que pisamos una casilla, si nunca la hemos pisado devuelve 0
double ComportamientoAuxiliar::obtenerTiempo(const pair<int, int> &pos){

	auto it = explored.find(pos);
	return (it != explored.end()) ? it->second : 0.0;

}

int ComportamientoAuxiliar::VeoCasillaInteresante_N0(char i, char c, char d, Sensores sensores){
	

	pair<int, int> pos = {sensores.posF, sensores.posC};

	pair<int, int> nextWALK = NextCasillaA_N01(pos, sensores.rumbo); // Siguiente casilla en caso de hacer WALK

	Orientacion izq = static_cast<Orientacion>((sensores.rumbo + 7) % 8); 
	pair<int, int> nextIzq = NextCasillaA_N01(pos, izq); // Siguiente casilla en caso de girar a la izquierda

	Orientacion der = static_cast<Orientacion>((sensores.rumbo + 1) % 8); 
	pair<int, int> nextDer = NextCasillaA_N01(pos, der); // Siguiente casilla en caso de girar a la derecha

	double tiempoWALK = obtenerTiempo(nextWALK);
	double tiempoIzq = obtenerTiempo(nextIzq);
	double tiempoDer = obtenerTiempo(nextDer);

	double minimoTiempo = min(tiempoWALK, min(tiempoIzq, tiempoDer));

	if(c == 'X') return 2;
	else if(i == 'X') return 1;
	else if(d == 'X') return 3;

	

	if(tiempoWALK == minimoTiempo && c == 'C') return 2;
	else if(tiempoIzq == minimoTiempo && i == 'C') return 1;
	else if(tiempoDer == minimoTiempo && d == 'C') return 3;



	if(c == 'C') return 2;
	else if(i == 'C') return 1;
	else if(d == 'C') return 3;

	return 0; // Si no hay casillas interesantes

}

char ComportamientoAuxiliar::ViablePorAltura(char casilla, int dif){

	/*if(abs(dif) <= 1){
		return casilla;
	} else {
		return 'P';
	}*/

	return abs(dif) <= 1 ? casilla : 'P';

}


// Función para completar el mapaResultado y mapaCotas
void SituarSensorEnMapaA(vector<vector<unsigned char>> &m, vector<vector<unsigned char>> &a, Sensores sensores) {
    
	m[sensores.posF][sensores.posC] = sensores.superficie[0];
    a[sensores.posF][sensores.posC] = sensores.cota[0];

    int f_norte[] = {-1, -1, -1, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3};
    int c_norte[] = {-1,  0,  1, -2, -1,  0,  1,  2, -3, -2, -1,  0,  1,  2,  3};

	int f_noreste[] = {-1, -1, 0, -2, -2, -2, -1, 0, -3, -3, -3, -3, -2, -1, 0};
	int c_noreste[] = {0, 1, 1, 0, 1, 2, 2, 2, 0, 1, 2, 3, 3, 3, 3};

    for (int i = 0; i < 15; i++) {
        int nuevaFila = sensores.posF;
        int nuevaColumna = sensores.posC;

        switch (sensores.rumbo) {
            case norte:
                nuevaFila += f_norte[i];
                nuevaColumna += c_norte[i];
                break;
            case noreste:
                nuevaFila += f_noreste[i];
                nuevaColumna += c_noreste[i]; 
                break;
            case este:
                nuevaFila += c_norte[i];
                nuevaColumna -= f_norte[i]; 
                break;
            case sureste:
                nuevaFila += c_noreste[i];
                nuevaColumna -= f_noreste[i]; 
                break;
            case sur:
                nuevaFila -= f_norte[i];
                nuevaColumna -=  c_norte[i];
                break;
            case suroeste:
                nuevaFila -= f_noreste[i];
                nuevaColumna -= c_noreste[i]; 
                break;
            case oeste:
                nuevaFila -= c_norte[i];
                nuevaColumna += f_norte[i]; 
                break;
            case noroeste:
                nuevaFila -= c_noreste[i];
                nuevaColumna += f_noreste[i];
                break;
        }

        m[nuevaFila][nuevaColumna] = sensores.superficie[i + 1];
        a[nuevaFila][nuevaColumna] = sensores.cota[i + 1];
    }
}

/*
******************************************************
			FUNCIONES PARA EL NIVEL 1
******************************************************
*/


// Función para comprobar si la siguiente casilla ha sido visitada recientemente
bool ComportamientoAuxiliar::CasillaReciente(const deque<pair<int, int>> &recientes, const pair<int, int> &pos){
	
	for(int i = 0; i < recientes.size(); i++){
		if(recientes[i] == pos){
			return true;
		}
	}

	return false;

}




int ComportamientoAuxiliar::VeoCasillaInteresante_N1(char i, char c, char d, Sensores sensores){

	pair<int, int> pos = {sensores.posF, sensores.posC};

	pair<int, int> nextWALK = NextCasillaA_N01(pos, sensores.rumbo); // Siguiente casilla en caso de hacer WALK

	Orientacion izq = static_cast<Orientacion>((sensores.rumbo + 7) % 8);
	pair<int, int> nextIzq = NextCasillaA_N01(pos, izq); // Siguiente casilla en caso de girar a la izquierda

	Orientacion der = static_cast<Orientacion>((sensores.rumbo + 1) % 8);
	pair<int, int> nextDer = NextCasillaA_N01(pos, der); // Siguiente casilla en caso de girar a la derecha

	double tiempoWALK = obtenerTiempo(nextWALK);
	double tiempoIzq = obtenerTiempo(nextIzq);
	double tiempoDer = obtenerTiempo(nextDer);

	double minimoTiempo = min(tiempoWALK, min(tiempoIzq, tiempoDer));


	recientes.push_back(pos);
	if(recientes.size() > 15){
		recientes.pop_front();
	}


	if(sensores.energia <= 0.25 * 3000){
		if(c == 'X') return 2;
		else if(i == 'X') return 1;
		else if(d == 'X') return 3;
	}

	

	if(tiempoWALK == minimoTiempo && c == 'C' && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(tiempoIzq == minimoTiempo && i == 'C' && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(tiempoDer == minimoTiempo && d == 'C' && !CasillaReciente(recientes, nextDer)) return 3;

	if(tiempoWALK == minimoTiempo && c == 'S' && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(tiempoIzq == minimoTiempo && i == 'S' && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(tiempoDer == minimoTiempo && d == 'S' && !CasillaReciente(recientes, nextDer)) return 3;

	

	if(c == 'C' || c == 'S') return 2;
	else if(i == 'C' || i == 'S') return 1;
	else if(d == 'C' || d == 'S') return 3;

	

	return 0; // Si no hay casillas interesantes

}


/*
******************************************************
			FUNCIONES PARA EL NIVEL 3
******************************************************
*/


void ComportamientoAuxiliar::AnularMatriz(vector<vector<unsigned char>> &m){
	for(int i = 0; i < m.size(); i++){
		for(int j = 0; j < m[i].size(); j++){
			m[i][j] = 0;
		}
	}
}



// Función para calcular la siguiente casilla en función de la orientación
EstadoA ComportamientoAuxiliar::NextCasillaAuxiliar(const EstadoA &st){
	EstadoA next = st;

	switch (st.brujula)
	{
		case norte:
			next.f = st.f - 1;
			break;
		case noreste:
			next.f = st.f - 1;
			next.c = st.c + 1;
			break;
		case este:
			next.c = st.c + 1;
			break;
		case sureste:
			next.f = st.f + 1;
			next.c = st.c + 1;
			break;
		case sur:
			next.f = st.f + 1;
			break;
		case suroeste:
			next.f = st.f + 1;
			next.c = st.c - 1;
			break;
		case oeste:
			next.c = st.c - 1;
			break;
		case noroeste:
			next.f = st.f - 1;
			next.c = st.c - 1;
			break;
		default:
			break;

	}

	return next;

}

void ComportamientoAuxiliar::VisualizaPlan(const EstadoA &st, const list<Action> &plan){
	AnularMatriz(mapaConPlan);
	EstadoA cst = st;

	auto it = plan.begin();
	while(it != plan.end()){
		switch (*it){
			case WALK:
				cst = NextCasillaAuxiliar(cst);
				if(cst.f >= 0 && cst.f < mapaConPlan.size() && cst.c >= 0 && cst.c < mapaConPlan[0].size()){ // Verificamos los límites del mapa para evitar errores
					mapaConPlan[cst.f][cst.c] = 2; 
				}
				break;
			case TURN_SR:
				cst.brujula = (cst.brujula + 1) % 8;
				break;
		}
		it++;
	}
}

bool ComportamientoAuxiliar::CasillaAccesibleAuxiliar(const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){

	EstadoA next = NextCasillaAuxiliar(st);
	bool terrenoValido1 = false;
	bool terrenoValido2 = false;
	bool alturaAccesible = false;
	
	if(next.f < 0 || next.f >= terreno.size() || next.c < 0 || next.c >= terreno[0].size()){
		return false;
	}

	

	terrenoValido1 = terreno[next.f][next.c] != 'P' && terreno[next.f][next.c] != 'M';
	terrenoValido2 = terreno[next.f][next.c] != 'B' || (st.zapatillas && terreno[next.f][next.c] == 'B');

	bool terrenoValido = terrenoValido1 && terrenoValido2;

	if(terreno[next.f][next.c] == '?'){ // Para el nivel 4 si la casilla es desconocida la tratamos como accesible
		alturaAccesible = true;
	} else{
		alturaAccesible = abs(altura[next.f][next.c] - altura[st.f][st.c]) <= 1; 
	}
 
	return terrenoValido && alturaAccesible;
}



// Función para obtener el siguiente estado en función de la acción y del estado actual
EstadoA ComportamientoAuxiliar::applyA(Action accion, const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){

	EstadoA next = st;

	switch (accion) {
		case WALK: 
			if (CasillaAccesibleAuxiliar(st, terreno, altura)) {
				next = NextCasillaAuxiliar(st);
				next.zapatillas = (terreno[next.f][next.c] == 'D') ? true : st.zapatillas;
			}
			break;
		

		case TURN_SR:
			next.brujula = (next.brujula + 1) % 8;
			break;

		default:
			break;
	}

	return next;
}


void ComportamientoAuxiliar::PintaPlan(const list<Action> &plan, bool zap){
	auto it = plan.begin();
	while (it != plan.end()){
		if (*it == WALK){
			cout << "W ";
		} else if (*it == TURN_SR){
			cout << "r ";
		} else {
			cout << "-_ ";
		}
		it++;
	}
	cout << "( longitud " << plan.size();
	if (zap) cout << "[Z]";
	cout << ")\n";
}


// Heurística de Chebychev para el algoritmo A*
int ComportamientoAuxiliar::heuristica(const EstadoA &inicio, const EstadoA &final){
	
	int max_x = abs(final.f - inicio.f);
	int max_y = abs(final.c - inicio.c);
	int h = max_x > max_y ? max_x : max_y;
	return h;
}


// Función para calcular el coste según la acción y la casilla actual, basándonos en los costes proporcionados en el guión	
int ComportamientoAuxiliar::CalcularCoste(Action accion, const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
	
	int coste = 0;
	int incremento = 0;

	int f = st.f;
	int c = st.c;

	int dif = 0;

	EstadoA next = NextCasillaAuxiliar(st);
	
	int f_next = next.f;
	int c_next = next.c;

	if(f_next < 0 || f_next >= terreno.size() || c_next < 0 || c_next >= terreno[0].size())
		return INT_MAX;
	

	dif = (altura[f_next][c_next] - altura[f][c]);
	
	

	unsigned char casilla = terreno[f][c]; // Para el nivel 4 si la casilla es desconocida la tratamos como casilla accesible de tipo camino

	switch(accion){
		case WALK:
			switch(casilla){
				case 'A': coste = 100; incremento = 10; break;
				case 'T': coste = 20; incremento = 5; break;
				case 'S': coste = 2; incremento = 1; break;
				case 'B':
					if(st.zapatillas){
						coste = 1; incremento = 0;
					} else {
						coste = INT_MAX;
					}
					break;
				default: coste = 1; incremento = 0; break;
			}

			if(dif > 0){
				if(dif <= 1) coste += incremento;
				else coste = INT_MAX;
			} else if(dif < 0){
				if(abs(dif) <= 1) coste -= incremento;
				else coste = INT_MAX;
			}

			break;
		
		case TURN_SR:
			switch(casilla){
				case 'A': coste = 16; break;
				case 'T': coste = 3; break;
				case 'S': coste = 1; break;
				case 'B':
					if(st.zapatillas){
						coste = 1;
					} else {
						coste = INT_MAX;
					}
				default: coste = 1; break;
			}
			break;

		default:
			break;
	}


	return coste;
}


// Función para calcular el camino óptimo en energía utilizando A*
list<Action> ComportamientoAuxiliar::A_estrellaAuxiliar(const EstadoA &inicio, const EstadoA &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    
	priority_queue<NodoA> frontier;
    map<EstadoA, int> explored;  
    list<Action> path;

    NodoA current_node;
    current_node.estado = inicio;
    current_node.coste = 0;
    current_node.coste_heuristica = heuristica(inicio, final);

    frontier.push(current_node);

    bool SolutionFound = (current_node.estado.f == final.f && current_node.estado.c == final.c);

    while (!frontier.empty() && !SolutionFound) {
        
		current_node = frontier.top();
        frontier.pop();

		// Si el nodo ya ha sido explorado y su coste es mayor o igual al coste almacenado, lo ignoramos
        if(explored.find(current_node.estado) != explored.end() && current_node.coste >= explored[current_node.estado]) {
			continue; 
		}

		explored[current_node.estado] = current_node.coste;


		if (current_node.estado.f == final.f && current_node.estado.c == final.c) {
			SolutionFound = true; // Si llegamos al nodo destino tenemos la solución
		} else{

			// Vector de acciones posibles para el auxiliar
			Action acciones[] = {WALK, TURN_SR};

			int coste;
			EstadoA estado;
			NodoA child;

			for(Action accion : acciones){
				
				coste = CalcularCoste(accion, current_node.estado, terreno, altura);
				if(coste == INT_MAX) continue;

				estado = applyA(accion, current_node.estado, terreno, altura);
				if(estado == current_node.estado) continue;

				child.estado = estado;

				child.coste = current_node.coste + coste;
				child.coste_heuristica = heuristica(child.estado, final);

				child.secuencia = current_node.secuencia;
				child.secuencia.push_back(accion);
				
				if(explored.find(child.estado) == explored.end() || child.coste < explored[child.estado]){
					frontier.push(child);
				}

			}

		}
	}

    if (SolutionFound) {
        path = current_node.secuencia;
        PintaPlan(path, current_node.estado.zapatillas);
    }

    return path;



}


/*
******************************************************
			FUNCIONES PARA EL NIVEL 4
******************************************************
*/



// Calculamos la distancia a la base más cercana para recargar energía
pair<int, int> ComportamientoAuxiliar::baseMasCercana(Sensores sensores){


	pair<int, int> pos = {sensores.posF, sensores.posC};
	pair<int, int> base;

	int distancia = INT_MAX;

	for(int i = 0; i < mapaResultado.size(); i++){
		for(int j = 0; j < mapaResultado[i].size(); j++){
			if(mapaResultado[i][j] == 'X'){
				int d = max(abs(i - pos.first), abs(j - pos.second));
				if(d < distancia){
					distancia = d;
					base = {i, j};
				}
			}
		}
	}

	return base;

}



// Comprobamos si el tiempo de bloqueo de una casilla por un agente ha sido superado
bool ComportamientoAuxiliar::tiempoSuperado(Sensores sensores, const pair<int, int> &pos){

	
	if(sensores.agentes[2] != '_' && tiempoBloqueo < maxTiempoBloqueo){
		tiempoBloqueo++;
		return true;
	} else{
		tiempoBloqueo = 0;
		return false;
	}
	

	return false;

}


// Si se ha superado el tiempo de bloqueo, bloqueamos la casilla y la marcamos como 'M'
void ComportamientoAuxiliar::bloquearCasilla(const pair<int, int> &pos, const Sensores &sensores, vector<vector<unsigned char>> &terreno){

	
	if(tiempoSuperado(sensores, pos)){
		hayPlan = false;
		plan.clear();
		pair<int, int> next = NextCasillaA_N01(pos, sensores.rumbo);

		if(next.first >= 0 && next.first < terreno.size() && next.second >= 0 && next.second < terreno[0].size()){
			terreno[next.first][next.second] = 'M';
		}
	}



}


// Comprobamos si el accidentado y el agente rescatador están en el cono de visión del auxiliar
bool ComportamientoAuxiliar::veAccidentado(Sensores sensores){

	int df = abs(sensores.destinoF - sensores.posF);
	int dc = abs(sensores.destinoC - sensores.posC);

	bool veAcc = false;
	for(int i = 1; i <= 15 && !veAcc; i++){
		if(sensores.agentes[i] == 'a'){
			veAcc = true;
		}
	}

	return (df <= 3 && dc <= 3 && veAcc);

}


bool vandaloExcursionistaA(Sensores sensores){

	for(int i = 1; i <= 15; i++){
		if(sensores.agentes[i] == 'v' || sensores.agentes[i] == 'e'){
			return true;
		}
	}

	return false;

}

// Casillas viables para el auxiliar
bool ComportamientoAuxiliar::casillasViables(const char casilla){

	return (casilla == 'X' || casilla == 'D' || casilla == 'C' || casilla == 'S' || (casilla == 'B' && tiene_zapatillas));

}


int ComportamientoAuxiliar::CasillaInteresante_N4(const char i, const char c, const char d, bool zap, Sensores sensores){

	pair<int, int> pos = {sensores.posF, sensores.posC};

	pair<int, int> nextWALK = NextCasillaA_N01(pos, sensores.rumbo);

	Orientacion izq = static_cast<Orientacion>((sensores.rumbo + 7) % 8);
	pair<int, int> nextIzq = NextCasillaA_N01(pos, izq);

	Orientacion der = static_cast<Orientacion>((sensores.rumbo + 1) % 8);
	pair<int, int> nextDer = NextCasillaA_N01(pos, der);

	double tiempoWALK = obtenerTiempo(nextWALK);
	double tiempoIzq = obtenerTiempo(nextIzq);
	double tiempoDer = obtenerTiempo(nextDer);

	double minimoTiempo = min(tiempoWALK, min(tiempoIzq, tiempoDer));


	recientes.push_back(pos);
	if(recientes.size() > 15){
		recientes.pop_front();
	}



	if(tiempoWALK == minimoTiempo && casillasViables(c) && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(tiempoIzq == minimoTiempo && casillasViables(i) && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(tiempoDer == minimoTiempo && casillasViables(d) && !CasillaReciente(recientes, nextDer)) return 3;

	if(consideraOtras){

		
		if(tiempoWALK == minimoTiempo && c == 'T' && !CasillaReciente(recientes, nextWALK)) return 2;
		else if(tiempoIzq == minimoTiempo && i == 'T' && !CasillaReciente(recientes, nextIzq)) return 1;
		else if(tiempoDer == minimoTiempo && d == 'T' && !CasillaReciente(recientes, nextDer)) return 3;

		if(tiempoWALK == minimoTiempo && c == 'A' && !CasillaReciente(recientes, nextWALK)) return 2;
		else if(tiempoIzq == minimoTiempo && i == 'A' && !CasillaReciente(recientes, nextIzq)) return 1;
		else if(tiempoDer == minimoTiempo && d == 'A' && !CasillaReciente(recientes, nextDer)) return 3;


	}



	if(casillasViables(c)) return 2;
	else if(casillasViables(i)) return 1;
	else if(casillasViables(d)) return 3;


	if(consideraOtras){

		if(c == 'A' || c == 'T') return 2;
		else if(i == 'A' || i == 'T') return 1;
		else if(d == 'A' || d == 'T') return 3;


	}

	return 0;

}



// Si no conseguimos un camino viable al accidentado, buscamos un camino viable a una casilla cercana
list<Action> ComportamientoAuxiliar::OtrosCaminos(const EstadoA &inicio, const EstadoA &final) {
    const int f_original = final.f;
    const int c_original = final.c;
    
    vector<pair<int, int>> puntos;

    for(int i = 1; i <= 3; i++) {
        for(int j = -i; j <= i; j++) {  
            if(abs(j) == i) {
                puntos.push_back({j, -i});
                puntos.push_back({j, i});
            } else {
                puntos.push_back({-i, j});
                puntos.push_back({i, j});
            }
        }
    }

    sort(puntos.begin(), puntos.end());
    puntos.erase(unique(puntos.begin(), puntos.end()), puntos.end());

    for(const auto& punto : puntos) {
        int f_alternativo = f_original + punto.first;
        int c_alternativo = c_original + punto.second;

        if(f_alternativo >= 0 && f_alternativo < mapaResultado.size() && c_alternativo >= 0 && c_alternativo < mapaResultado[0].size()) {
            
            EstadoA nuevo = {f_alternativo, c_alternativo};
            
            list<Action> plan_alternativo = A_estrellaAuxiliar(inicio, nuevo, mapaResultado, mapaCotas);
            
            if(!plan_alternativo.empty()) {
                VisualizaPlan(inicio, plan_alternativo);
                return plan_alternativo;
            }
        }
    }

    return {}; 
}




// Función de exploración en caso de que la acción más prioritaria sea explorar
Action ComportamientoAuxiliar::AuxiliarReactivo_N4(Sensores sensores){

	Action accion = IDLE;

	SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);


	if(last_action == WALK){
		pair<int, int> posicionActual = {sensores.posF, sensores.posC};
		explored[posicionActual] = sensores.tiempo;
	}

	if(sensores.superficie[0] == 'A' || sensores.superficie[0] == 'T'){
		consideraOtras = true;
	} 

	if(sensores.superficie[0] == 'D'){
		tiene_zapatillas = true;
	} 

	if(giro45Izq != 0){
		accion = TURN_SR;
		giro45Izq--;
	} else if(agenteVisibleA(sensores)){
		giro45Izq = 3;
		accion = TURN_SR;
	} else {

		char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
		char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
		char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

		int pos = CasillaInteresante_N4(i, c, d, tiene_zapatillas, sensores);
		
		switch(pos){
			case 2:
				accion = WALK;
							
				break;
			case 1:
				giro45Izq = 6;
				accion = TURN_SR;
	
				break;
			case 3:
				accion = TURN_SR;
	
				break;
			case 0:

				giro45Izq = 4;
				accion = TURN_SR;

	
				break;
	
			default:
				break;	
			}
		
	}

	


	last_action = accion;
	return accion;


}


// Función para volver a la base más cercana en caso de que la acción más prioritaria sea volver a la base
Action ComportamientoAuxiliar::volviendoABase(Sensores sensores){

	bool valido = true;
	EstadoA inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
	pair<int, int> base = baseMasCercana(sensores);
	EstadoA final = {base.first, base.second};

	if(base.first == -1 && base.second == -1){
		return AuxiliarReactivo_N4(sensores);
	}

	
	if(!hayPlan || plan.empty()){

		plan = A_estrellaAuxiliar(inicio, final, mapaResultado, mapaCotas);

		if(plan.empty()){
			plan = OtrosCaminos(inicio, final);
		}

		hayPlan = plan.size() != 0;
		VisualizaPlan(inicio, plan);

	}

	if(hayPlan && !plan.empty()){
		Action accion = plan.front();
		plan.pop_front();

		if(vandaloExcursionistaA(sensores)){
			
			hayPlan = false;
			plan.clear();
			accion = IDLE;
		}

		if(accion == WALK){
			bloquearCasilla({sensores.posF, sensores.posC}, sensores, mapaResultado);
			valido = CasillaAccesibleAuxiliar(inicio, mapaResultado, mapaCotas);
		}

		if(!valido){
			hayPlan = false;
			plan.clear();
			accion = IDLE;
		}

		last_action = accion;
		return accion;
	} else{
		buscaBase = false;
		hayPlan = false;
		plan.clear();
		last_action = IDLE;
		return IDLE;
	}


}


// Función para rescatar al accidentado en caso de que la acción más prioritaria sea rescatar
Action ComportamientoAuxiliar::aPorHerido(Sensores sensores){

	bool valido = true;
	EstadoA inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
	EstadoA final = {sensores.destinoF, sensores.destinoC};

	if(!hayPlan || plan.empty()){
		plan = A_estrellaAuxiliar(inicio, final, mapaResultado, mapaCotas);

		if(plan.empty()){
			plan = OtrosCaminos(inicio, final);
		}

		hayPlan = plan.size() != 0;
		VisualizaPlan(inicio, plan);
	}

	if(hayPlan && !plan.empty()){
		Action accion = plan.front();
		plan.pop_front();


		
		if(accion == WALK){
			bloquearCasilla({sensores.posF, sensores.posC}, sensores, mapaResultado);
			valido = CasillaAccesibleAuxiliar(inicio, mapaResultado, mapaCotas);
		}

		if(!valido){
			hayPlan = false;
			plan.clear();
			accion = IDLE;
		}

		last_action = accion;
		return accion;
	} else{
		hayPlan = false;
		plan.clear();
		last_action = IDLE;
		return IDLE;
	}


}


/*
******************************************************
			FUNCIONES PARA EL COMPORTAMIENTO
******************************************************
*/


Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_0(Sensores sensores)
{
	Action accion;

	SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);

	static int countWALK = 0;
	
	if(last_action == WALK){

		pair<int, int> posicionActual = {sensores.posF, sensores.posC};
		explored[posicionActual] = sensores.tiempo;

	}
	
	if(sensores.superficie[0] == 'D'){
		tiene_zapatillas = true;
	}

	if(sensores.superficie[0] == 'X'){
		accion = IDLE;
	} else if(giro45Izq != 0){ 
		accion = TURN_SR;
		giro45Izq--;
	} else if (agenteVisibleA(sensores)) {
		giro45Izq = 5;
		accion = TURN_SR;

	}else {

		char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
		char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
		char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

		int pos = VeoCasillaInteresante_N0(i, c, d, sensores);

		switch (pos){
		case 2:
			if(countWALK < 15){ // Al superar los 15 WALKS, forzamos el giro para mejorar la exploración
				accion = WALK;
				countWALK++;
			} else{
				countWALK = 0;
				giro45Izq = 1;
				accion = TURN_SR;
				
			}


			break;
		case 1:
			giro45Izq = 6;
			accion = TURN_SR;

			break;
		case 3:
			accion = TURN_SR;

			break;
		case 0:
			giro45Izq = 5;
			accion = TURN_SR;

			break;

		default:
			break;
		}
	}


	last_action = accion;
	return accion;



}



Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_1(Sensores sensores)
{	

	Action accion;

	SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);
	static int countWALK = 0;

	energiaSuficiente = sensores.energia >= 1500;
	
	if(last_action == WALK){
		pair<int, int> posicionActual = {sensores.posF, sensores.posC};
		explored[posicionActual] = sensores.tiempo;

	} 

	
	if(sensores.superficie[0] == 'D'){
		tiene_zapatillas = true;
	}

	if(sensores.superficie[0] == 'X' && !energiaSuficiente){ // Si la energía no es suficiente esperamos en una casilla base
		accion = IDLE;
	} else if(giro45Izq != 0){ 
		accion = TURN_SR;
		giro45Izq--;
	} else if (agenteVisibleA(sensores)) {
		giro45Izq = 4;
		accion = TURN_SR;

	}else {

		char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
		char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
		char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

		int pos = VeoCasillaInteresante_N1(i, c, d, sensores);

		switch (pos){
			case 2:
				if(countWALK < 20){ // Si hacemos 20 WALKS seguidos, forzamos el giro para mejorar la exploración
					accion = WALK;
					countWALK++;
				} else{
					countWALK = 0;
					giro45Izq = 1;
					accion = TURN_SR;
				}

	
				break;
			case 1:
				countWALK = 0;
				giro45Izq = 6;
				accion = TURN_SR;
	
				break;
			case 3:
				countWALK = 0;
				accion = TURN_SR;
	
				break;
			case 0:
				countWALK = 0;
				giro45Izq = 5;
				accion = TURN_SR;
	
				break;
	
			default:
				break;
			}
	}

	



	last_action = accion;
	return accion;
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_2(Sensores sensores)
{
	return IDLE;
}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_3(Sensores sensores)
{

	Action accion;
	if(!hayPlan){

		if(sensores.superficie[0] == 'D'){
			tiene_zapatillas = true;
		}
		
		EstadoA inicio;
		inicio.f = sensores.posF;
		inicio.c = sensores.posC;
		inicio.brujula = sensores.rumbo;
		inicio.zapatillas = tiene_zapatillas;

		EstadoA final;
		final.f = sensores.destinoF;
		final.c = sensores.destinoC;

		plan = A_estrellaAuxiliar(inicio, final, mapaResultado, mapaCotas);
		VisualizaPlan(inicio, plan);
		hayPlan = plan.size() != 0;

	}
	if(hayPlan && plan.size() > 0){
		accion = plan.front();
		plan.pop_front();
	} else {
		accion = IDLE;
	}

	return accion;

}

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_4(Sensores sensores)
{
	
	Action accion = IDLE;

	const int umbralEnergia = 600;

	SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);

	if(sensores.superficie[0] == 'D'){
		tiene_zapatillas = true;
	}


	if(sensores.energia > 2500){
		return AuxiliarReactivo_N4(sensores);
	}

	
	if(sensores.energia < umbralEnergia){
		buscaBase = true;
		hayPlan = false;
		plan.clear();
	}

	if(sensores.energia > 1500 && buscaBase){
		buscaBase = false;
		hayPlan = false;
		plan.clear();
	}

	if(sensores.superficie[0] == 'X' && sensores.energia < 1500){
		return IDLE;
	}

	

	if(buscaBase){
		return volviendoABase(sensores);
	}

	if(sensores.venpaca){
		return aPorHerido(sensores);
	}
	
	if(veAccidentado(sensores)){
		buscaBase = false;
		hayPlan = false;
		plan.clear();
		sensores.destinoF = -1;
		sensores.destinoC = -1;
		return IDLE;
	}



	if(sensores.destinoF == -1 || sensores.destinoC == -1){
		return AuxiliarReactivo_N4(sensores);
	}

	return IDLE;

}