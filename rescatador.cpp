#include "../Comportamientos_Jugador/rescatador.hpp"
#include "motorlib/util.h"

Action ComportamientoRescatador::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoRescatadorNivel_0 (sensores);
		break;
	case 1:
		accion = ComportamientoRescatadorNivel_1 (sensores);
		break;
	case 2:
		accion = ComportamientoRescatadorNivel_2 (sensores);
		break;
	case 3:
		accion = ComportamientoRescatadorNivel_3 (sensores);
		break;
	case 4:
		accion = ComportamientoRescatadorNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoRescatador::interact(Action accion, int valor)
{
	return 0;
}


/*
******************************************************
			FUNCIONES PARA EL NIVEL 0
******************************************************
*/


// Función auxiliar para calcular la siguiente casilla en función de la orientación
pair<int, int> NextCasillaR_N01(const pair<int, int> &st, Orientacion rumbo){

	pair<int, int> next = st;

	switch (rumbo)
	{
	
		case norte:
			next.first--;
			break;
		case noreste:
			next.first--;
			next.second++;
			break;
		case este:
			next.second++;
			break;
		case sureste:
			next.first++;
			next.second++;
			break;
		case sur:
			next.first++;
			break;
		case suroeste:
			next.first++;
			next.second--;
			break;
		case oeste:
			next.second--;
			break;
		case noroeste:
			next.first--;
			next.second--;
			break;
		default:
			break;

	}

	return next;

}

// Función para detectar algún agente en el cono de visión
bool agenteVisibleR(Sensores sensores){

	for(int i=1; i<=8; i++){
		if(sensores.agentes[i] != '_'){
			return true;
			
		}
	}

	return false;

}

// Método para obtener la última vez que pisamos una casilla, si nunca la hemos pisado devuelve 0
double ComportamientoRescatador::obtenerTiempo(const pair<int, int> &pos){

	auto it = explored.find(pos);
	return (it != explored.end()) ? it->second : 0.0;

}


int ComportamientoRescatador::VeoCasillaInteresante_N0(char i, char c, char d, bool zap, Sensores sensores){
	
	pair<int, int> pos = {sensores.posF, sensores.posC};

	pair<int, int> nextWALK = NextCasillaR_N01(pos, sensores.rumbo); // Siguiente casilla en caso de hacer WALK

	Orientacion izq = static_cast<Orientacion>((sensores.rumbo + 7) % 8); 
	pair<int, int> nextIzq = NextCasillaR_N01(pos, izq); // Siguiente casilla en caso de girar a la izquierda

	Orientacion der = static_cast<Orientacion>((sensores.rumbo + 1) % 8); 
	pair<int, int> nextDer = NextCasillaR_N01(pos, der); // Siguiente casilla en caso de girar a la derecha

	double tiempoWALK = obtenerTiempo(nextWALK);
	double tiempoIzq = obtenerTiempo(nextIzq);
	double tiempoDer = obtenerTiempo(nextDer);

	double minimoTiempo = min(tiempoWALK, min(tiempoIzq, tiempoDer));

	if(c == 'X') return 2;
	else if(i == 'X') return 1;
	else if(d == 'X') return 3;

	if(c == 'D' && !zap) return 2;
	else if(i == 'D' && !zap) return 1;
	else if(d == 'D' && !zap) return 3;

	

	if(tiempoWALK == minimoTiempo && c == 'C') return 2;
	else if(tiempoIzq == minimoTiempo && i == 'C') return 1;
	else if(tiempoDer == minimoTiempo && d == 'C') return 3;

	if(tiempoWALK == minimoTiempo && c == 'D') return 2;
	else if(tiempoIzq == minimoTiempo && i == 'D') return 1;
	else if(tiempoDer == minimoTiempo && d == 'D') return 3;



	if(c == 'C' || c == 'D') return 2;
	else if(i == 'C' || i == 'D' ) return 1;
	else if(d == 'C' || d == 'D') return 3;

	return 0; // Si no hay casillas interesantes

}

char ComportamientoRescatador::ViablePorAltura(char casilla, int dif, bool zap){

	/*if(abs(dif) <= 1 || (zap && abs(dif) <= 2)){
		return casilla;
	} else{
		return 'P';
	}*/

	return (abs(dif) <= 1 || (zap && abs(dif) <= 2)) ? casilla : 'P';

}


// Función para completar el mapaResultado y mapaCotas
void SituarSensorEnMapaR(vector<vector<unsigned char>> &m, vector<vector<unsigned char>> &a, Sensores sensores) {
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
bool ComportamientoRescatador::CasillaReciente(const deque<pair<int, int>> &recientes, const pair<int, int> &pos){
	
	for(int i = 0; i < recientes.size(); i++){
		if(recientes[i] == pos){
			return true;
		}
	}

	return false;

}



int ComportamientoRescatador::VeoCasillaInteresante_N1(char i, char c, char d, bool zap, Sensores sensores){

	pair<int, int> pos = {sensores.posF, sensores.posC};

	pair<int, int> nextWALK = NextCasillaR_N01(pos, sensores.rumbo); // Siguiente casilla en caso de hacer WALK

	Orientacion izq = static_cast<Orientacion>((sensores.rumbo + 7) % 8);
	pair<int, int> nextIzq = NextCasillaR_N01(pos, izq); // Siguiente casilla en caso de girar a la izquierda

	Orientacion der = static_cast<Orientacion>((sensores.rumbo + 1) % 8);
	pair<int, int> nextDer = NextCasillaR_N01(pos, der); // Siguiente casilla en caso de girar a la derecha

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



	if(c == 'D' && !zap) return 2;
	else if(i == 'D' && !zap) return 1;
	else if(d == 'D' && !zap) return 3;
	


	if(tiempoWALK == minimoTiempo && c == 'C' && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(tiempoIzq == minimoTiempo && i == 'C' && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(tiempoDer == minimoTiempo && d == 'C' && !CasillaReciente(recientes, nextDer)) return 3;

	if(tiempoWALK == minimoTiempo && c == 'D' && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(tiempoIzq == minimoTiempo && i == 'D' && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(tiempoDer == minimoTiempo && d == 'D' && !CasillaReciente(recientes, nextDer)) return 3;

	if(tiempoWALK == minimoTiempo && c == 'S' && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(tiempoIzq == minimoTiempo && i == 'S' && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(tiempoDer == minimoTiempo && d == 'S' && !CasillaReciente(recientes, nextDer)) return 3;

	

	if(c == 'C' || c == 'D' || c == 'S') return 2;
	else if(i == 'C' || i == 'D' || i == 'S') return 1;
	else if(d == 'C' || d == 'D' || d == 'S') return 3;

	


	return 0; // Si no hay casillas interesantes

}

/*
******************************************************
			FUNCIONES PARA EL NIVEL 2
******************************************************
*/

void ComportamientoRescatador::AnularMatriz(vector<vector<unsigned char>> &m){
	for(int i = 0; i < m.size(); i++){
		for(int j = 0; j < m[i].size(); j++){
			m[i][j] = 0;
		}
	}
}


// Función para calcular la siguiente casilla en función de la orientación
EstadoR ComportamientoRescatador::NextCasillaRescatador(const EstadoR &st){

	EstadoR next = st;

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


void ComportamientoRescatador::VisualizaPlan(const EstadoR &st, const list<Action> &plan){
	AnularMatriz(mapaConPlan);
	EstadoR cst = st;

	auto it = plan.begin();
	while(it != plan.end()){
		switch (*it){
			case RUN:
				cst = NextCasillaRescatador(cst);
				if(cst.f >= 0 && cst.f < mapaConPlan.size() && cst.c >= 0 && cst.c < mapaConPlan[0].size()){ // Verificamos los límites del mapa para evitar errores
					mapaConPlan[cst.f][cst.c] = 3;
					cst = NextCasillaRescatador(cst);
					if(cst.f >= 0 && cst.f < mapaConPlan.size() && cst.c >= 0 && cst.c < mapaConPlan[0].size()){
						mapaConPlan[cst.f][cst.c] = 3;
					}	
				}
				
				break;
			case WALK:
				cst = NextCasillaRescatador(cst);
				if(cst.f >= 0 && cst.f < mapaConPlan.size() && cst.c >= 0 && cst.c < mapaConPlan[0].size()){
					mapaConPlan[cst.f][cst.c] = 1;
				}
				break;
			case TURN_SR:
				cst.brujula = (cst.brujula + 1) % 8;
				break;
			case TURN_L:
				cst.brujula = (cst.brujula + 6) % 8;
				break;
		}
		it++;
	}
}


bool ComportamientoRescatador::CasillaAccesibleRescatador(const EstadoR &st, Action accion, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){

	EstadoR next = NextCasillaRescatador(st);

	if(next.f < 0 || next.f >= terreno.size() || next.c < 0 || next.c >= terreno[0].size()){
		return false;
	}

	if(accion == RUN){ // Si la acción es RUN, comprobamos la casilla intermedia y la casilla final
		EstadoR intermedio = next;
		EstadoR final = NextCasillaRescatador(intermedio);

		if(final.f < 0 || final.f >= terreno.size() || final.c < 0 || final.c >= terreno[0].size()){
			return false;
		}



		bool terreno_intermedio = (terreno[intermedio.f][intermedio.c] != 'P' && terreno[intermedio.f][intermedio.c] != 'M' && terreno[intermedio.f][intermedio.c] != 'B');
		bool terreno_final = (terreno[final.f][final.c] != 'P' && terreno[final.f][final.c] != 'M' && terreno[final.f][final.c] != 'B');
		bool terrenoAccesible = terreno_intermedio && terreno_final;

		bool alturaAccesible;
		if(terreno[intermedio.f][intermedio.c] == '?'){ // Para el nivel 4 si la casilla es desconocida la tratamos como accesible
			alturaAccesible = true;
		} else{
			alturaAccesible = abs(altura[final.f][final.c] - altura[st.f][st.c]) <= 1 || (st.zapatillas && abs(altura[final.f][final.c] - altura[st.f][st.c]) <= 2);
		}
		

		return terrenoAccesible && alturaAccesible;

	} else{

		bool terrenoAccesible = (terreno[next.f][next.c] != 'P' && terreno[next.f][next.c] != 'M' && terreno[next.f][next.c] != 'B');
		
		bool alturaAccesible;

		if(terreno[next.f][next.c] == '?'){ // Para el nivel 4 si la casilla es desconocida la tratamos como accesible
			alturaAccesible = true;
		} else{
			alturaAccesible = abs(altura[next.f][next.c] - altura[st.f][st.c]) <= 1 || (st.zapatillas && abs(altura[next.f][next.c] - altura[st.f][st.c]) <= 2);
		}
	

		return terrenoAccesible && alturaAccesible;

	}

}


// Función para obtener el siguiente estado en función de la acción y del estado actual
EstadoR ComportamientoRescatador::applyR(Action accion, const EstadoR &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){

	EstadoR next = st;
	EstadoR intermedio = st;

	switch(accion){
		case WALK:
			if(CasillaAccesibleRescatador(st, accion, terreno, altura)){


				next = NextCasillaRescatador(st);
				next.zapatillas = (terreno[next.f][next.c] == 'D') ? true : st.zapatillas;

				
			}
			break;

		case RUN:
			if(CasillaAccesibleRescatador(st, accion, terreno, altura)){
				intermedio = NextCasillaRescatador(st);
				next = NextCasillaRescatador(intermedio);

				next.zapatillas = (terreno[next.f][next.c] == 'D') ? true : intermedio.zapatillas;

				
			} else {
				next = st;
			}
			break;

		case TURN_L:
			next.brujula = (st.brujula + 6) % 8;
			break;

		case TURN_SR:
			next.brujula = (st.brujula + 1) % 8;
			break;
	}

	return next;
}

	
void ComportamientoRescatador::PintaPlan(const list<Action> &plan, bool zap){

	auto it = plan.begin();
	while (it != plan.end()){
		if (*it == WALK){
			cout << "W ";
		} else if (*it == RUN){
			cout << "R ";
		} else if (*it == TURN_SR){
			cout << "r ";
		} else if (*it == TURN_L){
			cout << "L ";
		} else if (*it == CALL_ON){
			cout << "C ";
		} else if (*it == CALL_OFF){
			cout << "c ";
		} else if (*it == IDLE){
			cout << "I ";
		} else{
			cout << "-_ ";
		}
		it++;
	}
	cout << "( longitud " << plan.size();
	if (zap) cout << "[Z]";
	cout << ")\n";

}


// Función para calcular el coste según la acción y la casilla actual, basándonos en los costes proporcionados en el guión	
int ComportamientoRescatador::CalcularCoste(Action accion, const EstadoR &st, vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura){

	int coste = 0;
	int incremento = 0;

	int f = st.f;
	int c = st.c;

	EstadoR next = NextCasillaRescatador(st);
	EstadoR next_run = NextCasillaRescatador(next);

	int f_next = next.f;
	int c_next = next.c;

	int f_run = next_run.f;
	int c_run = next_run.c;

	if (f_next < 0 || f_next >= terreno.size() || c_next < 0 || c_next >= terreno[0].size())
		return INT_MAX;

	if (f_run < 0 || f_run >= terreno.size() || c_run < 0 || c_run >= terreno[0].size())
		return INT_MAX;

	int dif = (altura[f_next][c_next] - altura[f][c]);
	int dif_run = altura[f_run][c_run] - altura[f][c];

	unsigned char casilla = terreno[f][c]; // Para el nivel 4 si la casilla es desconocida la tratamos como casilla accesible de tipo camino


	switch(accion){
		case WALK:
			switch(casilla){
				case 'A': coste = 100; incremento = 10; break;
				case 'T': coste = 20; incremento = 5; break;
				case 'S': coste = 2; incremento = 1; break;
				default: coste = 1; incremento = 0; break;
			}

			if(dif > 0){
				if((dif <= 2 && st.zapatillas) || dif<=1) coste += incremento;
				else coste = INT_MAX; // Si la diferencia de altura es mayor a 2 es una casilla inaccesible
			} else if(dif < 0){
				if((abs(dif) <= 2 && st.zapatillas) || abs(dif)<=1) coste -= incremento;
				else coste = INT_MAX;
			}
			

			break;
		
		case RUN:
			switch(casilla){
				case 'A': coste = 150; incremento = 15; break;
				case 'T': coste = 35; incremento = 5; break;
				case 'S': coste = 3; incremento = 2; break;
				default: coste = 1; incremento = 0; break;
			}

			if(dif_run > 0){
				if((dif_run <= 2 && st.zapatillas) || dif_run <= 1) coste += incremento;
				else coste = INT_MAX;
			} else if(dif_run < 0){
				if((abs(dif_run) <= 2 && st.zapatillas) || abs(dif_run) <= 1) coste -= incremento;
				else coste = INT_MAX;
			}

			break;

		case TURN_L:
			switch(casilla){
				case 'A': coste = 30; break;
				case 'T': coste = 5; break;
				case 'S': coste = 1; break;
				default: coste = 1; break;
			}
			break;

		case TURN_SR:
			switch(casilla){
				case 'A': coste = 16; break;
				case 'T': coste = 3; break;
				case 'S': coste = 1; break;
				default: coste = 1; break;
			}
			break;

		default:
			break;
	}


	return coste;
}


// Función para calcular el camino óptimo en energía utilizando Dijkstra
list<Action> ComportamientoRescatador::DijkstraRescatador(const EstadoR &inicio, const EstadoR &final, vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura){

	priority_queue<NodoR> frontier;
	map<EstadoR, int> explored;
	list<Action> path;

	NodoR current_node;
	current_node.estado = inicio;
	current_node.coste = 0;
	frontier.push(current_node);

	bool SolutionFound = (current_node.estado.f == final.f && current_node.estado.c == final.c);

	while(!frontier.empty() && !SolutionFound){

		current_node = frontier.top();
		frontier.pop();

		// Si el nodo ya ha sido explorado y su coste es mayor o igual al coste almacenado, lo ignoramos
		if(explored.find(current_node.estado) != explored.end() && current_node.coste >= explored[current_node.estado]){ 
			continue;
		}


		explored[current_node.estado] = current_node.coste;

		if(current_node.estado.f == final.f && current_node.estado.c == final.c){
			SolutionFound = true; // Si llegamos al nodo destino tenemos la solución
		} else{

			// Vector de acciones posibles para el rescatador
			Action acciones[] = {WALK, RUN, TURN_L, TURN_SR};

			for(Action accion : acciones){

				int coste = CalcularCoste(accion, current_node.estado, terreno, altura);

				if(coste == INT_MAX) continue;

				EstadoR estado = applyR(accion, current_node.estado, terreno, altura);

				if(estado == current_node.estado) continue;
			
				
				NodoR child;
				child.estado = estado;
				child.coste = current_node.coste + coste;
				child.secuencia = current_node.secuencia;
				child.secuencia.push_back(accion);

				
				if(explored.find(child.estado) == explored.end() || child.coste < explored[child.estado]){
					frontier.push(child);
				}


			}

		}

		

	}

	

	if(SolutionFound){
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
pair<int, int> ComportamientoRescatador::baseMasCercana(Sensores sensores){
	
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
bool ComportamientoRescatador::tiempoSuperado(Sensores sensores, const pair<int, int> &pos, const Action &accion){

	if(accion == WALK){
		if(sensores.agentes[2] != '_' && tiempoBloqueo < maxTiempoBloqueo){
			tiempoBloqueo++;
			return true;
		} else{
			tiempoBloqueo = 0;
			return false;
		}
	}

	if(accion == RUN){
		if(sensores.agentes[2] != '_' && sensores.agentes[6] != '_' && tiempoBloqueo < maxTiempoBloqueo){
			tiempoBloqueo++;
			return true;
		} else{
			tiempoBloqueo = 0;
			return false;
		}
	}

	return false;

}


// Si se ha superado el tiempo de bloqueo, bloqueamos la casilla y la marcamos como 'M'
void ComportamientoRescatador::bloquearCasilla(const pair<int, int> &pos, const Sensores &sensores, const Action &accion, vector<vector<unsigned char>> &terreno){

	if(accion == WALK){
		if(tiempoSuperado(sensores, pos, accion)){
			hayPlan = false;
			plan.clear();
			pair<int, int> next = NextCasillaR_N01(pos, sensores.rumbo);

			if(next.first >= 0 && next.first < terreno.size() && next.second >= 0 && next.second < terreno[0].size()){
				terreno[next.first][next.second] = 'M';
			}
		}
	}

	if(accion == RUN){
		if(tiempoSuperado(sensores, pos, accion)){
			hayPlan = false;
			plan.clear();
			pair<int, int> next = NextCasillaR_N01(pos, sensores.rumbo);

			if(next.first >= 0 && next.first < terreno.size() && next.second >= 0 && next.second < terreno[0].size()){
				terreno[next.first][next.second] = 'M';
			}

			pair<int, int> next2 = NextCasillaR_N01(next, sensores.rumbo);

			if(next2.first >= 0 && next2.first < terreno.size() && next2.second >= 0 && next2.second < terreno[0].size()){
				terreno[next2.first][next2.second] = 'M';
			}
		}
	}
	

}



bool vandaloExcurisionistaR(Sensores sensores){

	for(int i=1; i<=15; i++){
		if(sensores.agentes[i] == 'v' || sensores.agentes[i] == 'e'){
			return true;
			
		}
	}

	return false;

}


// Casillas viables para el rescatador
bool ComportamientoRescatador::casillasViables(const char casilla){

	return (casilla == 'X' || casilla == 'D' || casilla == 'C' || casilla == 'S');

}

// Casilla viables para el rescatador solo si aparece en una de ellas o es empujado por un vandalo
bool OtrasCasillasViables(const char casilla){
	return casilla == 'A' || casilla == 'T';
}


int ComportamientoRescatador::CasillaInteresante_N4(const char i, const char c, const char d, bool zap, Sensores sensores){

	pair<int, int> pos = {sensores.posF, sensores.posC};

	pair<int, int> nextWALK = NextCasillaR_N01(pos, sensores.rumbo);

	Orientacion izq = static_cast<Orientacion>((sensores.rumbo + 7) % 8);
	pair<int, int> nextIzq = NextCasillaR_N01(pos, izq);

	Orientacion der = static_cast<Orientacion>((sensores.rumbo + 1) % 8);
	pair<int, int> nextDer = NextCasillaR_N01(pos, der);

	double tiempoWALK = obtenerTiempo(nextWALK);
	double tiempoIzq = obtenerTiempo(nextIzq);
	double tiempoDer = obtenerTiempo(nextDer);

	double minimoTiempo = min(tiempoWALK, min(tiempoIzq, tiempoDer));

	recientes.push_back(pos);
	if(recientes.size() > 15){
		recientes.pop_front();
	}


	
	if(casillasViables(c) && minimoTiempo == tiempoWALK && !CasillaReciente(recientes, nextWALK)) return 2;
	else if(casillasViables(i) && minimoTiempo == tiempoIzq && !CasillaReciente(recientes, nextIzq)) return 1;
	else if(casillasViables(d) && minimoTiempo == tiempoDer && !CasillaReciente(recientes, nextDer)) return 3;


	if(consideraOtras){

		if(OtrasCasillasViables(c) && minimoTiempo == tiempoWALK && !CasillaReciente(recientes, nextWALK)) return 2;
		else if(OtrasCasillasViables(i) && minimoTiempo == tiempoIzq && !CasillaReciente(recientes, nextIzq)) return 1;
		else if(OtrasCasillasViables(d) && minimoTiempo == tiempoDer && !CasillaReciente(recientes, nextDer)) return 3;

	}



	if(casillasViables(c)) return 2;
	else if(casillasViables(i)) return 1;
	else if(casillasViables(d)) return 3;


	if(consideraOtras){

		if(OtrasCasillasViables(c)) return 2;
		else if(OtrasCasillasViables(i)) return 1;
		else if(OtrasCasillasViables(d)) return 3;

	}

	return 0;


}


// Si no conseguimos un camino viable al accidentado, buscamos un camino viable a una casilla cercana
list<Action> ComportamientoRescatador::OtrosCaminos(const EstadoR &inicio, const EstadoR &final) {
    int f_original = final.f;
    int c_original = final.c;
    
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
            
            EstadoR nuevo = {f_alternativo, c_alternativo};
            
            list<Action> plan_alternativo = DijkstraRescatador(inicio, nuevo, mapaResultado, mapaCotas);
            
            if(!plan_alternativo.empty()) {
                VisualizaPlan(inicio, plan_alternativo);
                return plan_alternativo;
            }
        }
    }

    return {}; 
}




// Función de exploración en caso de que la acción más prioritaria sea explorar
Action ComportamientoRescatador::RescatadorReactivo_N4(Sensores sensores){

	Action accion;

	SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);



	if(last_action == WALK || last_action == RUN){
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
	} else if(agenteVisibleR(sensores)){
		giro45Izq = 3;
		accion = TURN_SR;
	} else {

		char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
		char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
		char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

		int pos = CasillaInteresante_N4(i, c, d, tiene_zapatillas, sensores);
		
		switch(pos){
			case 2:
				accion = WALK;
							
				break;
			case 1:
				giro45Izq = 1;
				accion = TURN_L;
	
				break;
			case 3:
				accion = TURN_SR;
	
				break;
			case 0:

				if(last_action != TURN_L){
					giro45Izq = 1;
					accion = TURN_SR;
				} else{
					accion = TURN_L;
				}

	
				break;
	
			default:
				break;	
			}
		
	}

	


	last_action = accion;
	return accion;



}


// Función para volver a la base más cercana en caso de que la acción más prioritaria sea volver a la base
Action ComportamientoRescatador::volviendoABase(Sensores sensores){

	bool valido = true;
	EstadoR inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
	pair<int, int> base = baseMasCercana(sensores);
	EstadoR final = {base.first, base.second};

	if(base.first == -1 && base.second == -1){
		return RescatadorReactivo_N4(sensores);
	}

	

	if(!hayPlan || plan.empty()){

		plan = DijkstraRescatador(inicio, final, mapaResultado, mapaCotas);

		if(plan.empty()){
			plan = OtrosCaminos(inicio, final);
		}

		hayPlan = plan.size() != 0;
		VisualizaPlan(inicio, plan);

	}

	if(hayPlan && !plan.empty()){
		Action accion = plan.front();
		plan.pop_front();


		if(vandaloExcurisionistaR(sensores)){
			
			hayPlan = false;
			plan.clear();
			accion = TURN_L;
		}
		
		if (accion == WALK || accion == RUN) {
			bloquearCasilla({sensores.posF, sensores.posC}, sensores, accion, mapaResultado);
			valido = CasillaAccesibleRescatador(inicio, accion, mapaResultado, mapaCotas);
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
Action ComportamientoRescatador::aPorHerido(Sensores sensores){
bool valido = true;
	EstadoR inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
	EstadoR final = {sensores.destinoF, sensores.destinoC};

	if(!hayPlan || plan.empty()){
		plan = DijkstraRescatador(inicio, final, mapaResultado, mapaCotas);

		if(plan.empty()){
			plan = OtrosCaminos(inicio, final);
		}

		hayPlan = plan.size() != 0;
		VisualizaPlan(inicio, plan);
	}

	if(hayPlan && !plan.empty()){
		Action accion = plan.front();
		plan.pop_front();

		

		if (accion == WALK || accion == RUN) {
			bloquearCasilla({sensores.posF, sensores.posC}, sensores, accion, mapaResultado);
			valido = CasillaAccesibleRescatador(inicio, accion, mapaResultado, mapaCotas);
		}

		if(!valido){
			hayPlan = false;
			plan.clear();
			return IDLE;
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


Action ComportamientoRescatador::ComportamientoRescatadorNivel_0(Sensores sensores)
{
	Action accion;

	SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);

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
	} else if(agenteVisibleR(sensores)){
		accion = TURN_L;
	} else {

		char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
		char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
		char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

		int pos = VeoCasillaInteresante_N0(i, c, d, tiene_zapatillas, sensores);
		
		switch(pos){
		case 2:
			if(countWALK < 15){ // Al superar los 15 WALKS, forzamos el giro para mejorar la exploración
				countWALK++;
				accion = WALK;
			} else{
				countWALK = 0;
				accion = TURN_L;
				
			}


						
			break;
		case 1:
			giro45Izq = 1;
			accion = TURN_L;

			break;
		case 3:
			accion = TURN_SR;

			break;
		case 0:
			if(last_action != TURN_L){
				giro45Izq = 1;
				accion = TURN_SR;
			} else{
				accion = TURN_L;
			}

			break;

		default:
			break;	
		}
		
	}

	

	last_action = accion;
	return accion;

}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_1(Sensores sensores)
{
	
	Action accion;

	SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);
	static int countWALK = 0;	

	energiaSuficiente = sensores.energia >= 1500;

	if(last_action == WALK || last_action == RUN){
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
	} else if(agenteVisibleR(sensores)){
		giro45Izq = 1;
		accion = TURN_SR;
	} else {

		char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
		char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
		char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

		int pos = VeoCasillaInteresante_N1(i, c, d, tiene_zapatillas, sensores);
		
		switch(pos){
			case 2:

				if(countWALK < 20){ // Si hacemos 20 WALKS seguidos, forzamos el giro para mejorar la exploración
					countWALK++;
					accion = WALK;
				} else{
					countWALK = 0;
					accion = TURN_L;
				}

							
				break;
			case 1:
				countWALK = 0;
				giro45Izq = 1;
				accion = TURN_L;
	
				break;
			case 3:
				countWALK = 0;
				accion = TURN_SR;
	
				break;
			case 0:

				countWALK = 0;
				if(last_action != TURN_L){
					giro45Izq = 1;
					accion = TURN_SR;
				} else{
					accion = TURN_L;
				}

	
				break;
			
			default:
				break;	
			}
		
	}

	


	last_action = accion;
	return accion;
}

Action ComportamientoRescatador::ComportamientoRescatadorNivel_2(Sensores sensores)
{

	Action accion;

	
	if(!hayPlan){

		if(sensores.superficie[0] == 'D'){
			tiene_zapatillas = true;
		}

		EstadoR inicio;
		inicio.f = sensores.posF;
		inicio.c = sensores.posC;
		inicio.brujula = sensores.rumbo;
		inicio.zapatillas = tiene_zapatillas;

		EstadoR final;
		final.f = sensores.destinoF;
		final.c = sensores.destinoC;

		plan = DijkstraRescatador(inicio, final, mapaResultado, mapaCotas);
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

Action ComportamientoRescatador::ComportamientoRescatadorNivel_3(Sensores sensores)
{
	return IDLE;
}


Action ComportamientoRescatador::ComportamientoRescatadorNivel_4(Sensores sensores) {


	Action accion = IDLE;

	const int umbralEnergia = 600;

	SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);

	if(sensores.superficie[0] == 'D'){
		tiene_zapatillas = true;
	}




	if(destinoAnterior.first != sensores.destinoF || destinoAnterior.second != sensores.destinoC){
		destinoAnterior = {sensores.destinoF, sensores.destinoC};
		hayPlan = false;
		plan.clear();
		buscaBase = false;
		call_on = false;
	}


	if(sensores.energia > 2500){
		return RescatadorReactivo_N4(sensores);
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



	if(sensores.posF == sensores.destinoF && sensores.posC == sensores.destinoC){
		if(sensores.gravedad && !call_on){
			call_on = true;
			return CALL_ON;
		} else if(sensores.gravedad && call_on){ 
			return IDLE;
		} else{
			call_on = false;
			hayPlan = false;
			plan.clear();
			return CALL_OFF;
		}
	}


	if(buscaBase){
		return volviendoABase(sensores);
	} else{
		return aPorHerido(sensores);
	}

}
