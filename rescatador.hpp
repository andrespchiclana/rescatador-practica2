#ifndef COMPORTAMIENTORESCATADOR_H
#define COMPORTAMIENTORESCATADOR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <iostream>
#include <queue>
#include <map>
#include <climits>
#include <algorithm>

#include "comportamientos/comportamiento.hpp"


struct EstadoR{
  int f, c;
  int brujula;
  bool zapatillas;

  bool operator<(const EstadoR &st) const {
    return (f < st.f || (f == st.f && c < st.c) || (f == st.f && c == st.c && brujula < st.brujula) || (f == st.f && c == st.c && brujula == st.brujula && zapatillas < st.zapatillas));
  }

  bool operator>(const EstadoR &st) const{
    return !(*this < st);
  }

  bool operator==(const EstadoR &st) const {
    return (f == st.f && c == st.c && brujula == st.brujula && zapatillas == st.zapatillas);
  }

  bool operator!=(const EstadoR &st) const {
    return !(*this == st);
  }

  void operator=(const EstadoR &st) {
    f = st.f;
    c = st.c;
    brujula = st.brujula;
    zapatillas = st.zapatillas;
  }
};

struct NodoR{
  EstadoR estado;
  list<Action> secuencia;
  int coste;

  bool operator<(const NodoR &nodo) const {
    return (coste > nodo.coste) || (coste == nodo.coste && estado < nodo.estado) || (coste == nodo.coste && estado == nodo.estado && secuencia.size() < nodo.secuencia.size());
  }

  bool operator>(const NodoR &nodo) const {
    return !(*this < nodo);
  }

  bool operator==(const NodoR &node) const {
    return estado == node.estado;
  }

  bool operator!=(const NodoR &node) const {
    return !(*this == node);
  }

  
  
};

class ComportamientoRescatador : public Comportamiento
{

public:
  ComportamientoRescatador(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4

    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;
    explored = map<pair<int, int>, double>();
    recientes = deque<pair<int, int>>();
    energiaSuficiente = true;

    buscaBase = false;
    consideraOtras = false;
    tiempoBloqueo = 0;
    call_on = false;
    destinoAnterior = {-1, -1};


    plan = list<Action>();
    hayPlan = false;

    for(int i = 0; i < size; i++){ // Para el nivel 4 es necesario inicializar el mapaCotas para poder realizar plan
      for(int j = 0; j < size; j++){
        mapaCotas[i][j] = '0';
      }
    }
     
  }
  ComportamientoRescatador(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR,mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;
    plan = list<Action>();
    hayPlan = false;

  }
  ComportamientoRescatador(const ComportamientoRescatador &comport) : Comportamiento(comport) {}
  ~ComportamientoRescatador() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  //FUNCIONES PARA EL NIVEL 0
  double obtenerTiempo(const pair<int, int> &pos);
  int VeoCasillaInteresante_N0(char i, char c, char d, bool zap, Sensores sensores);
  char ViablePorAltura(char casilla, int dif, bool zap);

  //FUNCIONES PARA EL NIVEL 1
  bool CasillaReciente(const deque<pair<int, int>> &recientes, const pair<int, int> &pos);
  bool runPosible(Sensores sensores, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  int VeoCasillaInteresante_N1(char i, char c, char d, bool zap, Sensores sensores);

  //FUNCIONES PARA EL NIVEL 2
  void AnularMatriz(vector<vector<unsigned char>> &m);
  EstadoR NextCasillaRescatador(const EstadoR &st);
  void VisualizaPlan(const EstadoR &st, const list<Action> &plan);
  bool CasillaAccesibleRescatador(const EstadoR &st, Action accion, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoR applyR(Action accion, const EstadoR &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  void PintaPlan(const list<Action> &plan, bool zap);
  int CalcularCoste(Action accion, const EstadoR &st, vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura);
  list<Action> DijkstraRescatador(const EstadoR &inicio, const EstadoR &final, vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura);

  //FUNCIONES PARA EL NIVEL 4
  pair<int, int> baseMasCercana(Sensores sensores);
  bool tiempoSuperado(Sensores sensores, const pair<int, int> &pos, const Action &accion);
  void bloquearCasilla(const pair<int, int> &pos, const Sensores &sensores, const Action &accion, vector<vector<unsigned char>> &terreno);
  bool casillasViables(const char casilla);
  int CasillaInteresante_N4(const char i, const char c, const char d, bool zap, Sensores sensores);


  list<Action> OtrosCaminos(const EstadoR &inicio, const EstadoR &final);
  pair<int, int> BuscarX();


  Action RescatadorReactivo_N4(Sensores sensores);
  Action volviendoABase(Sensores sensores);
  Action aPorHerido(Sensores sensores);


  //FUNCIONES DE COMPORTAMIENTO
  Action ComportamientoRescatadorNivel_0(Sensores sensores);
  Action ComportamientoRescatadorNivel_1(Sensores sensores);
  Action ComportamientoRescatadorNivel_2(Sensores sensores);
  Action ComportamientoRescatadorNivel_3(Sensores sensores);
  Action ComportamientoRescatadorNivel_4(Sensores sensores);

private:
  // Variables de Estado para los niveles 0,1,4

  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;
  map<pair<int, int>, double> explored;
  deque<pair<int, int>> recientes;
  bool energiaSuficiente;

  bool buscaBase;
  bool consideraOtras;
  int tiempoBloqueo;
  const static int maxTiempoBloqueo = 3;
  bool call_on;
  pair<int, int> destinoAnterior;


  // Variables de Estado para los niveles 2 y 3
  list<Action> plan;
  bool hayPlan;

};



#endif
