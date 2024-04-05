#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
using namespace std;
using namespace std::chrono;
class Graph
{
private:
    int V;          // Número de vértices
    list<int> *adj; // Lista de adjacência
     vector<vector<int>> aux;
    // Função auxiliar para encontrar ciclos
    void findCycles(int v, int w, list<int> *adj, bool *visited, int *path, int pathIndex)
    {
        path[pathIndex++] = w; // Adiciona o vértice ao caminho
        // Se o vértice de destino foi alcançado e o caminho possui mais de 3 vértices
        if (v == w && pathIndex > 3)
        {
            // // Imprime o ciclo encontrado
            // for (int i = 0; i < pathIndex - 1; i++)
            // {
            //     char ch = 'A' + path[i];
            //     cout << ch << "-> ";
            // }
            // cout << "A" << endl;
        }
        else
        {
            // Loop sobre os vizinhos do vértice atual
            for (std::list<int>::iterator i = adj[w].begin(); i != adj[w].end(); i++)
            {
                // Se o vizinho não foi visitado e não é o vértice de partida
                if (!visited[*i] && (pathIndex > 2 || v != *i))
                {
                    visited[*i] = true; // Marca o vizinho como visitado
                    // Chama a função recursiva para continuar a busca
                    findCycles(v, *i, adj, visited, path, pathIndex);
                    visited[*i] = false; // Desmarca o vizinho como visitado
                }
            }
        }
    }

    bool isCycle(int *permutation, int v)
    {
        bool found = false; // flag
        for (std::list<int>::iterator j = adj[v].begin(); j != adj[v].end() && !found; j++)
        {
            // verifica se o primeiro elemento adjacente v é v
            if (permutation[0] == *j)
            {
                found = true;
            }
        }
        // se primeiro elemento adjacente é v retorna falso
        if (!found)
        {
            return false;
        }
        // verifica se ha aresta de u para w
        for (int i = 0; i < V; i++)
        {
            int u = permutation[i];
            int w = permutation[i + 1];
            found = false;
            if (u == v)
            {
                if (i > 1)
                {
                    return true;
                }
                return false;
            }
            else
            {
                for (std::list<int>::iterator j = adj[u].begin(); j != adj[u].end() && !found; j++)
                {
                    if (w == *j)
                    {
                        found = true;
                    }
                }
                if (!found)
                {
                    return false;
                }
            }
        }
        return true;
    }
    void swap(int *arr, int i, int j)
    {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    void permutation(int *array, int start, int end, int v)
    {
        if (start == end)
        {
            if (isCycle(array, v))
            {
                vector<int> tmp;
                for (int i = 0; array[i] != v; i++)
                {
                    tmp.push_back(array[i]);   
                }
                aux.push_back(tmp);
            }
            return;
        }
        for (int i = start; i <= end; ++i)
        {
            swap(array, start, i);
            permutation(array, start + 1, end, v);
            swap(array, start, i);
        }
    }

public:
    // Construtor da classe Graph
    Graph(int V)
    {
        this->V = V;            // Inicializa o número de vértices
        adj = new list<int>[V]; // Inicializa a lista de adjacência para cada vértice
    }

    // Função para adicionar uma aresta entre dois vértices
    void addEdge(int v, int w)
    {
        adj[v].push_back(w); // Adiciona w à lista de adjacência de v
        adj[w].push_back(v); // Adiciona v à lista de adjacência de w
    }

    // Função para encontrar ciclos no grafo
    void findCycles(int v)
    {
        int *path = new int[V];      // Caminho atual
        bool *visited = new bool[V]; // Vetor de visitados
        for (int i = 0; i < V; i++)
        {
            visited[i] = false; // Inicializa todos os vértices como não visitados
        }

        path[0] = v; // Inicia o caminho com o vértice de partida
        // Loop sobre os vizinhos do vértice de partida
        for (std::list<int>::iterator i = adj[v].begin(); i != adj[v].end(); i++)
        {
            int pathIndex = 1;                                // Reinicia o índice do caminho atual
            visited[*i] = true;                               // Marca o vértice como visitado
            findCycles(v, *i, adj, visited, path, pathIndex); // Chama a função recursiva para encontrar ciclos
            visited[*i] = false;                              // Desmarca o vértice como visitado
        }
    }

    void enumerateCycles(int v)
    {
        int *array = new int[V];
        for (int i = 0; i < V; i++)
        {
            array[i] = i;
        }
        swap(array, v, array[V - 1]);
        permutation(array, 0, V - 1, v);
       // Ordenar o vetor para facilitar a remoção de duplicatas
        std::sort(aux.begin(), aux.end());

        // Remover duplicatas usando unique e erase
        aux.erase(std::unique(aux.begin(), aux.end()), aux.end());
    }
};

void addRandomEdges(Graph& g, int V, double density) {
    int maxEdges = V * (V - 1) / 2;
    int edgesToAdd = static_cast<int>(maxEdges * density);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, V - 1);

    for (int i = 0; i < edgesToAdd; ++i) {
        int u = distrib(gen);
        int v = distrib(gen);
        if (u != v) {
            g.addEdge(u, v);
        }
    }
}

void runExperiments(int V, double density, std::ofstream& outFile) {
    Graph g(V);
    addRandomEdges(g, V, density);

    // Medindo tempo para findCycles
    auto startFindCycles = std::chrono::high_resolution_clock::now();
    g.findCycles(0); // Assumindo que queremos iniciar do vértice 0
    auto stopFindCycles = std::chrono::high_resolution_clock::now();
    auto durationFindCycles = std::chrono::duration_cast<std::chrono::microseconds>(stopFindCycles - startFindCycles);

    // Medindo tempo para enumerateCycles
    auto startEnumerateCycles = std::chrono::high_resolution_clock::now();
    g.enumerateCycles(0); // Assumindo que queremos iniciar do vértice 0
    auto stopEnumerateCycles = std::chrono::high_resolution_clock::now();
    auto durationEnumerateCycles = std::chrono::duration_cast<std::chrono::microseconds>(stopEnumerateCycles - startEnumerateCycles);

    // Escrevendo os resultados em formato de tabela LaTeX no arquivo outFile
    outFile << V << " & "
            << std::fixed << std::setprecision(2) << density << " & "
            << durationFindCycles.count() << " & "
            << durationEnumerateCycles.count() << " \\\\" << std::endl;
}

// Função principal
int main()
{
    // // Cria um grafo conforme o diagrama fornecido
    // Graph g(6);
    // g.addEdge(0, 1);
    // g.addEdge(0, 3);
    // g.addEdge(0, 4);
    // g.addEdge(1, 4);
    // g.addEdge(1, 3);
    // g.addEdge(1, 2);
    // g.addEdge(3, 2);
    // g.addEdge(4, 2);
    // g.addEdge(2, 5);
    // g.addEdge(3, 5);
    // g.addEdge(4, 5);

    // char v = 'A';
    // cout << "Searching cycles by path of " << v
    //      << endl;
    // g.findCycles((v - 'A')); // Encontra ciclos no grafo
    // cout << "Searching for cycles by permutation of " << v
    //      << endl;
    // g.enumerateCycles((v - 'A')); // Encontra ciclos no grafo

    std::ofstream outFile("resultados.tex");

    // Escrevendo o cabeçalho da tabela LaTeX no arquivo
    outFile << "\\begin{table}[ht]" << std::endl
            << "\\centering" << std::endl
            << "\\begin{tabular}{|c|c|c|c|}" << std::endl
            << "\\hline" << std::endl
            << "Vértices & Densidade & Tempo para busca de ciclos (µs) & Tempo para Enumeração de Ciclos (µs) \\\\" << std::endl
            << "\\hline" << std::endl;

    // Aqui você chamaria runExperiments para diferentes valores de V e densidades
    for (int vertices = 4; vertices <= 12; ++vertices) {
        for (double density = 0.5; density <= 1.0; density += 0.5) {
            runExperiments(vertices, density, outFile);
        }
    }

    // Escrevendo o rodapé da tabela LaTeX no arquivo
    outFile << "\\hline" << std::endl
            << "\\end{tabular}" << std::endl
            << "\\caption{Comparação dos Tempos de Execução para Diferentes Quantidades de Vértices e Densidades.}" << std::endl
            << "\\label{table:execution_times}" << std::endl
            << "\\end{table}" << std::endl;

    outFile.close();
    return 0;
}
