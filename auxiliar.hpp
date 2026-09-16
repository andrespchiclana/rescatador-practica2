#ifndef COMPORTAMIENTOAUXILIAR_H
#define COMPORTAMIENTOAUXILIAR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <queue>
#include <map>
#include <climits>
#include <algorithm>

#include "comportamientos/comportamiento.hpp"



struct EstadoA{
  int f, c;
  int brujula;
  bool zapatillas;

  bool operator<(const EstadoA &st) const {
    return (f < st.f || (f == st.f && c < st.c) || (f == st.f && c == st.c && brujula < st.brujula) || (f == st.f && c == st.c && brujula == st.brujula && zapatillas < st.zapatillas));
  }

  bool operator>(const EstadoA &st) const{
    return !(*this < st);
  }

  bool operator==(const EstadoA &st) const {
    return (f == st.f && c == st.c && brujula == st.brujula && zapatillas == st.zapatillas);
  }

  bool operator!=(const EstadoA &st) const {
    return !(*this == st);
  }

  void operator=(const EstadoA &st) {
    f = st.f;
    c = st.c;
    brujula = st.brujula;
    zapatillas = st.zapatillas;
  }
};



struct NodoA{
  EstadoA estado;
  list<Action> secuencia;
  int coste;
  int coste_heuristica;

  bool operator<(const NodoA &nodo) const {
    return (coste + coste_heuristica > nodo.coste + nodo.coste_heuristica) || (coste + coste_heuristica == nodo.coste + nodo.coste_heuristica && estado < nodo.estado) || (coste + coste_heuristica == nodo.coste + nodo.coste_heuristica && estado == nodo.estado && secuencia.size() < nodo.secuencia.size());
  }

  bool operator>(const NodoA &nodo) const {
    return !(*this < nodo);
  }

  bool operator==(const NodoA &node) const {
    return (estado == node.estado && coste == node.coste && coste_heuristica == node.coste_heuristica);
  }

  bool operator!=(const NodoA &node) const {
    return !(*this == node);
  }

  
  
};



class ComportamientoAuxiliar : public Comportamiento
{

public:
  ComportamientoAuxiliar(unsigned int size = 0) : Comportamiento(size)
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

    plan = list<Action>();
    hayPlan = false;

    for(int i = 0; i < size; i++){ // Para el nivel 4 es necesario inicializar el mapaCotas para poder realizar plan
      for(int j = 0; j < size; j++){
        mapaCotas[i][j] = '0';
      }
    }

  }
  ComportamientoAuxiliar(std::vector<std::vector<unsigned char>> MapaAR, std::vector<std::vector<unsigned char>> MapaAC) : Comportamiento(MapaAR,MapaAC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;
    plan = list<Action>();
    hayPlan = false;

  }
  ComportamientoAuxiliar(const ComportamientoAuxiliar &comport) : Comportamiento(comport) {}
  ~ComportamientoAuxiliar() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  //FUNCIONES PARA EL NIVEL 0
  double obtenerTiempo(const pair<int, int> &pos);
  int VeoCasillaInteresante_N0(char i, char c, char d, Sensores sensores);
  char ViablePorAltura(char casilla, int dif);

  //FUNCIONES PARA EL NIVEL 1
  bool CasillaReciente(const deque<pair<int, int>> &recientes, const pair<int, int> &pos);
  int VeoCasillaInteresante_N1(char i, char c, char d, Sensores sensores);

  //FUNCIONES PARA EL NIVEL 3
  void AnularMatriz(vector<vector<unsigned char>> &m);
  EstadoA NextCasillaAuxiliar(const EstadoA &st);
  void VisualizaPlan(const EstadoA &st, const list<Action> &plan);
  bool CasillaAccesibleAuxiliar(const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoA applyA(Action accion, const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  void PintaPlan(const list<Action> &plan, bool zap);
  int heuristica(const EstadoA &inicio, const EstadoA &final);
  int CalcularCoste(Action accion, const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  list<Action> A_estrellaAuxiliar(const EstadoA &inicio, const EstadoA &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  //FUNCIONES PARA EL NIVEL 4
  pair<int, int> baseMasCercana(Sensores sensores);
  bool tiempoSuperado(Sensores sensores, const pair<int, int> &pos);
  void bloquearCasilla(const pair<int, int> &pos, const Sensores &sensores, vector<vector<unsigned char>> &terreno);
  bool veAccidentado(Sensores sensores);

  bool casillasViables(const char casilla);
  int CasillaInteresante_N4(const char i, const char c, const char d, bool zap, Sensores sensores);

  list<Action> OtrosCaminos(const EstadoA &inicio, const EstadoA &final);
  pair<int, int> BuscarX();


  Action AuxiliarReactivo_N4(Sensores sensores);
  Action volviendoABase(Sensores sensores);
  Action aPorHerido(Sensores sensores);

  //FUNCIONES DE COMPORTAMIENTO
  Action ComportamientoAuxiliarNivel_0(Sensores sensores);
  Action ComportamientoAuxiliarNivel_1(Sensores sensores);
  Action ComportamientoAuxiliarNivel_2(Sensores sensores);
  Action ComportamientoAuxiliarNivel_3(Sensores sensores);
  Action ComportamientoAuxiliarNivel_4(Sensores sensores);


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

  // Variables de Estado para los niveles 2 y 3
  list<Action> plan;
  bool hayPlan;
};



#endif
