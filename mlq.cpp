#include <iostream> //Maneja la entrada y salida de la terminal
#include <fstream> // Permite la lectura y escritura de archivos
#include <sstream> //Biblioteca para la conversion de cadenas
#include <vector> //Biblioteca para usar vectores dinamicos, es decir que su tamaño no es definido
#include <string> //Biblioteca para usar la clase std::string para la manipulacion de cadenas de texto

#define MAX_PROCESOS 20

class Proceso {
public:
    std::string etiqueta;
    int BT;  // Tiempo para la ejecucion
    int AT;  // Tiempo de llegada 
    int Q;   // Cola
    int Pr;  // Prioridad
    int WT;  // Tiempo de espera 
    int CT;  // Tiempo de finalización 
    int RT;  // Tiempo de respuesta 
    int TAT; // Tiempo de retorno 
    int remainingBT; // Tiempo restante del burst
    int startTime;   // Primer momento de ejecucion

    Proceso() : BT(0), AT(0), Q(0), Pr(0), WT(0), CT(0), RT(0), TAT(0), remainingBT(0), startTime(-1) {}
};

void rr3(std::vector<Proceso>& procesos, int* tiempo);
void rr5(std::vector<Proceso>& procesos, int* tiempo);
void fcfs(std::vector<Proceso>& procesos, int* tiempo);
void calcularTiempos(std::vector<Proceso>& procesos);

int main() {
    std::string nombreArchivoEntrada, nombreArchivoSalida;

// Aqui se pregunta por el nombre del archivo de entrada
    std::cout << "Introduce el nombre del archivo de entrada: ";
    std::cin >> nombreArchivoEntrada;
//Aquí se pregunta por el nombre del archivo de salida
    std::cout << "Introduce el nombre del archivo de salida: ";
    std::cin >> nombreArchivoSalida;


    std::ifstream archivoEntrada(nombreArchivoEntrada);
    std::ofstream archivoSalida(nombreArchivoSalida);

    if (!archivoEntrada.is_open() || !archivoSalida.is_open()) {
        std::cerr << "Error al abrir el archivo.\n";
        return 1;
    }

    std::vector<Proceso> procesos;
    std::string buffer;

    // Leer los procesos desde el archivo
    while (std::getline(archivoEntrada, buffer)) {
        if (buffer[0] == '#' || buffer.empty()) continue; // Saltar comentarios y líneas vacías
        std::istringstream iss(buffer);
        Proceso p;
        std::string token;

        std::getline(iss, p.etiqueta, ';');
        iss >> p.BT; iss.ignore();
        iss >> p.AT; iss.ignore();
        iss >> p.Q; iss.ignore();
        iss >> p.Pr;

        p.remainingBT = p.BT; // Inicializar tiempo restante
        procesos.push_back(p);
    }
    archivoEntrada.close();

    // Ejecutar algoritmos para cada cola
    int tiempo = 0;

    // RR q=3 para la cola 1
    rr3(procesos, &tiempo);
    // RR q=5 para la cola 1
    rr5(procesos, &tiempo);
    // FCFS para la cola 3 
    fcfs(procesos, &tiempo);

    // Calcular tiempos finales
    calcularTiempos(procesos);

    // Escribir la salida en el archivo
    archivoSalida << "# archivo: " << nombreArchivoEntrada << "\n";
    archivoSalida << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";
    for (const auto& proceso : procesos) {
        archivoSalida << proceso.etiqueta << ";" 
                      << proceso.BT << ";" 
                      << proceso.AT << ";" 
                      << proceso.Q << ";" 
                      << proceso.Pr << ";" 
                      << proceso.WT << ";" 
                      << proceso.CT << ";" 
                      << proceso.RT << ";" 
                      << proceso.TAT << "\n";
    }

    // Imprimir promedios al final del archivo
    float totalWT = 0, totalCT = 0, totalRT = 0, totalTAT = 0;
    for (const auto& proceso : procesos) {
        totalWT += proceso.WT;
        totalCT += proceso.CT;
        totalRT += proceso.RT;
        totalTAT += proceso.TAT;
    }

    int n = procesos.size();
    archivoSalida << "WT=" << (totalWT / n) << "; "
                  << "CT=" << (totalCT / n) << "; "
                  << "RT=" << (totalRT / n) << "; "
                  << "TAT=" << (totalTAT / n) << ";\n";

    archivoSalida.close();
    return 0;
}

void rr3(std::vector<Proceso>& procesos, int* tiempo) {
    int remaining = 0;

    // Aqui se cuenta los procesos en la cola 1
    for (const auto& proceso : procesos) {
        if (proceso.Q == 1) {
            remaining++;
        }
    }

    while (remaining > 0) {
        bool found = false; // Aqui se verifica que hay un proceso que se ejecuta

        for (auto& proceso : procesos) {
            if (proceso.remainingBT > 0 && proceso.AT <= *tiempo && proceso.Q == 1) {
                found = true; //proceso para ejecutar

                if (proceso.startTime == -1) {
                    proceso.startTime = *tiempo; //tiempo de inicio
                }

                if (proceso.remainingBT > 3) {
                    *tiempo += 3;
                    proceso.remainingBT -= 3;
                } else {
                    *tiempo += proceso.remainingBT;
                    proceso.CT = *tiempo;
                    proceso.remainingBT = 0;
                    remaining--;
                }
            }
        }

        if (!found) { // si no se encontró un proceso para ejecutar se avanza en el tiempo
            (*tiempo)++;
        }
    }
}

void rr5(std::vector<Proceso>& procesos, int* tiempo) {
    int remaining = 0;

    // Aqui se cuentan los procesos de la cola 2
    for (const auto& proceso : procesos) {
        if (proceso.Q == 2) {
            remaining++;
        }
    }

    while (remaining > 0) {
        bool found = false; // verificacion proceso que se ejecuta

        for (auto& proceso : procesos) {
            if (proceso.remainingBT > 0 && proceso.AT <= *tiempo && proceso.Q == 2) {
                found = true; // proceso para ejecutar

                if (proceso.startTime == -1) {
                    proceso.startTime = *tiempo; // tiempo de inicio
                }

                if (proceso.remainingBT > 5) {
                    *tiempo += 5;
                    proceso.remainingBT -= 5;
                } else {
                    *tiempo += proceso.remainingBT;
                    proceso.CT = *tiempo;
                    proceso.remainingBT = 0;
                    remaining--;
                }
            }
        }

        if (!found) { // Si no se encontró un proceso para ejecutar, avanzamos el tiempo
            (*tiempo)++;
        }
    }
}

void fcfs(std::vector<Proceso>& procesos, int* tiempo) {
    // Aqui se procede a procesar lo procesos en la cola 3
    for (auto& proceso : procesos) {
        if (proceso.Q == 3) {
            // se espera a que haya un timepo de llegada mayor
            if (*tiempo < proceso.AT) {
                *tiempo = proceso.AT; 
            }

            // tiempo de inicio
            proceso.startTime = *tiempo;

            //se actualiza el tiempo total con el BT del proceso
            *tiempo += proceso.BT; 
            proceso.CT = *tiempo; // Establecer el tiempo de finalización

            // Calculo de métricas
            proceso.TAT = proceso.CT - proceso.AT; // Calcular el tiempo de turnaround
            proceso.WT = proceso.TAT - proceso.BT; // Calcular el tiempo de espera
            proceso.RT = proceso.startTime - proceso.AT; // Calcular el tiempo de respuesta
        }
    }
}

void calcularTiempos(std::vector<Proceso>& procesos) {
    for (auto& proceso : procesos) {
        proceso.TAT = proceso.CT - proceso.AT;
        proceso.WT = proceso.TAT - proceso.BT;
        proceso.RT = proceso.startTime - proceso.AT;
    }
}
