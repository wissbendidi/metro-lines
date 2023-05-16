#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Définition de la nombre maximale des stations
#define MAX_STATIONS 304
// Définition de la taille maximale des arrêtes (edges)
#define MAX_EDGES 734
// Définition de la longueur maximale des noms de station
#define MAX_LINE_LENGTH 1024
// Temps nécessaire pour changer de station de métro tout en restant sur la même ligne
#define SAME_LINE_TIME 1
// Temps nécessaire pour changer de ligne de métro différente
#define DIFFERENT_LINE_TIME 5

// Structure pour stocker les données d'une station
typedef struct {
    char name[MAX_LINE_LENGTH]; // Nom de la station
    int id; // ID de la station
} Station;

// Structure pour stocker les données d'une arrête (edge)
typedef struct {
    int origin; // ID de la station d'origine de l'arrête
    int destination; // ID de la station de destination de l'arrête
    int line; // ID de la ligne de métro de l'arrête
} Edge;

// Structure pour stocker la liste d'adjacence de chaque station
typedef struct AdjListNode {
    int destination; // ID de la station adjacente
    int line; // ID de la ligne de métro entre les deux stations
    int weight; // Poids de l'arrête entre les deux stations
    struct AdjListNode* next; // Pointeur vers le prochain noeud de la liste d'adjacence
} AdjListNode;

// Structure pour stocker la liste d'adjacence de toutes les stations
typedef struct {
    AdjListNode* head; // Pointeur vers le premier noeud de la liste d'adjacence de la station
} AdjList;

// Structure pour stocker le graphe
typedef struct {
    AdjList* array; // Tableau des listes d'adjacence pour toutes les stations
    int num_stations; // Nombre total de stations dans le graphe
} Graph;



// Fonction pour analyser les données des stations à partir d'un fichier CSV
int read_stations(Station stations[]) {
    FILE* fp;
    char line[MAX_LINE_LENGTH];
    char* token;
    int count = 0;

    fp = fopen("Metro Paris Data - Stations.csv", "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Ignorer la ligne d'en-tête
    fgets(line, MAX_LINE_LENGTH, fp);
    // Analyser les données de chaque station à partir du fichier CSV
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL && count < MAX_STATIONS) {
        // Diviser la ligne en tokens en utilisant la virgule comme délimiteur
        token = strtok(line, ",");
        strncpy(stations[count].name, token, MAX_LINE_LENGTH);
        token = strtok(NULL, ",");
        stations[count].id = atoi(token) - 1;
        count++;
    }

    fclose(fp);
    return count;
}

// Fonction pour parser les données de metro à partir d'un fichier CSV
int read_edges(Edge edges[]) {
    FILE* fp;
    char line[MAX_LINE_LENGTH];
    char* token;
    int count = 0;

    fp = fopen("Metro Paris Data - Aretes.csv", "r");
    if (fp == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    // Ignorer la ligne d'en-tête
    fgets(line, MAX_LINE_LENGTH, fp);

    // Parser les données de bordure à partir du fichier CSV
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL && count < MAX_EDGES) {
        // Diviser la ligne en jetons à l'aide de la virgule comme délimiteur
        token = strtok(line, ",");
        edges[count].origin = atoi(token) ;
        token = strtok(NULL, ",");
        edges[count].destination = atoi(token);
        token = strtok(NULL, ",");
        edges[count].line = atoi(token);
        count++;
    }

    fclose(fp);
    return count;
}

// Fonction pour créer un nouveau noeud de liste d'adjacence
AdjListNode* new_adj_list_node(int destination, int line, int weight) {
    AdjListNode* new_node = (AdjListNode*)malloc(sizeof(AdjListNode));
    new_node->destination = destination;
    new_node->line = line;
    new_node->weight = weight;
    new_node->next = NULL;
    return new_node;
}

// Fonction pour ajouter une ligne de metro entre deux stations à un graphe
void add_edge(Graph* graph, int src, int dest, int weight) {
    // Ajouter une ligne de metro du source à la destination
    AdjListNode* node = (AdjListNode*)malloc(sizeof(AdjListNode));
    node->destination = dest;
    node->weight = weight;
    node->next = graph->array[src].head;
    graph->array[src].head = node;

    // Ajouter une ligne de metro de la destination au source 
    ///!!! Ici on l'enlève, parce que etes sont orientés, et c'est déjà inclus dans le fichier 
    ///csv toutes les orientations
    //node = (AdjListNode*)malloc(sizeof(AdjListNode));
    //node->destination = src;
    //node->weight = weight;
    //node->next = graph->array[dest].head;
    //graph->array[dest].head = node;
}


// Fonction pour créer un nouveau graphe
Graph* create_graph(int num_stations, Edge edges[], int num_edges) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_stations = num_stations;
    graph->array = (AdjList*)malloc(num_stations * sizeof(AdjList));

    // Initialiser les listes d'adjacence pour toutes les stations

    for (int i = 0; i < num_stations; i++) {
        graph->array[i].head = NULL;
    }

    // Ajouter les arrêtes au graphe
    for (int i = 0; i < num_edges; i++) {
        Edge edge = edges[i];
        int src = edge.origin - 1;  // soustraire 1 des identifiants de station
        int dest = edge.destination - 1;
        add_edge(graph, src, dest, edge.line);
    }

    return graph;
}


// Fonction pour afficher la liste des stations
void display_stations(Station stations[], int num_stations) {
    printf("Stations:\n");
    for (int i = 0; i < num_stations; i++) {
        printf("%d. %s\n", stations[i].id + 1, stations[i].name);
    }
}

// Fonction pour obtenir l'ID d'une station à partir de son nom
int get_station_id(Station stations[], int num_stations, char* name) {
    for (int i = 0; i < num_stations; i++) {
        if (strcmp(stations[i].name, name) == 0) {
            return stations[i].id;
        }
    }
    return -1;
}



// Fonction pour afficher le chemin le plus court entre deux stations
int min_distance(int dist[], int visited[], int n) {
    int min_dist = INT_MAX, min_index;
    for (int i = 0; i < n; i++) {
        if (!visited[i] && dist[i] <= min_dist) {
            min_dist = dist[i];
            min_index = i;
        }
    }
    return min_index;
}


// La fonction print_path permet d'afficher le chemin le plus court entre deux stations à partir du tableau prev, 
//qui contient le nœud parent de chaque nœud visité. Elle prend également en compte les lignes de metro
//et les poids des nœuds visités. 
//Elle met à jour la somme totale des poids des stations visités dans la variable total_weight.
void print_path(int prev[], int dest, Station stations[], int lines[], int weights[], int* total_weight) {
    if (prev[dest] == -1) {
        printf("%s", stations[dest].name);
        return;
    }
    int prev_dest = prev[dest];
    print_path(prev, prev_dest, stations, lines, weights, total_weight);
    printf(" --%d--> %s", lines[dest], stations[dest].name);
    //printf(" (%d)", weights[dest]);
    if (prev_dest != -1) {
        weights[prev_dest] += weights[dest];
        *total_weight = *total_weight + weights[dest];
    }
    else {
        weights[prev_dest] = weights[dest];
        *total_weight = *total_weight + weights[dest];
    }
}


// La fonction dijkstra est utilisée pour trouver le plus court chemin entre deux stations dans un graphe 
//représentant le réseau de métro de Paris. Elle prend en entrée un pointeur vers le graphe, 
//l'ID de la station de départ, l'ID de la station d'arrivée et un tableau de stations. 
//Elle utilise l'algorithme de Dijkstra pour calculer le plus court chemin entre les deux stations, 
//en prenant en compte le temps de trajet entre les stations. La fonction initialise les distances 
//à partir de la station de départ, puis met à jour les distances des stations voisines non visitées 
//en fonction du temps de trajet et de la distance actuelle, en gardant une trace des stations précédentes 
//pour reconstruire le chemin final. Enfin, elle affiche le chemin le plus court et la durée totale du trajet en minutes.
void dijkstra(Graph* graph, int start_id, int end_id, Station stations[]){
    int dist[MAX_STATIONS], visited[MAX_STATIONS], prev[MAX_STATIONS],lines[MAX_STATIONS],weights[MAX_STATIONS];
    int w,prev_w;
    int total_weight = 0;


    // Step 1: Initialize the distance of the source node to 0 and the distance of all other nodes to infinity
    for (int i = 0; i < MAX_STATIONS; i++) {
        dist[i] = INT_MAX;
        visited[i] = 0;
        prev[i] = -1;
        lines[i] = 0;
    }
    dist[start_id] = 0;

    // Step 4: Find the node with the minimum distance among the unvisited nodes and update the distances of its neighbors
    for (int count = 0; count < MAX_STATIONS - 1; count++) {
        int u = min_distance(dist, visited, MAX_STATIONS);
        visited[u] = 1;
        AdjListNode* node = graph->array[u].head;
        prev_w = lines[u];
        // For each unvisited neighbor v of u, if the distance 
        // from the source to v through u is less than the current distance of v, update the distance of v
        while (node != NULL){
            if (prev_w == 0){
                    w = SAME_LINE_TIME;
                }
                else if (prev_w == node->weight){
                    w = SAME_LINE_TIME;
                }
                
                else{
                    w = DIFFERENT_LINE_TIME;
                }
            if (!visited[node->destination] && dist[u] != INT_MAX && dist[u] + w < dist[node->destination]) {
                dist[node->destination] = dist[u] + w;
                prev[node->destination] = u;
                lines[node->destination] = node->weight;
                weights[node->destination] = w;
            }
            node = node->next;
        }
        if (u == end_id) {
            break;
        }
    }

    // Step 5: Print the shortest path from source to destination
    printf("Le trajet le plus court de %s à %s: ", stations[start_id].name, stations[end_id].name);
    print_path(prev, end_id,stations,lines,weights,&total_weight);
    printf("\n");
    printf("\nLe trajet prend: %d minutes\n", total_weight);

}


//La fonction free_graph prend un pointeur vers un objet Graph et libère tous les nœuds d'adjacence 
//de toutes les listes d'adjacence de chaque station, ainsi que la mémoire allouée pour le tableau de 
//listes d'adjacence et l'objet Graph.
void free_graph(Graph* graph) {
    int i;
    AdjList* adj_list;
    AdjListNode* adj_node;

    for (i = 0; i < graph->num_stations; i++) {
        adj_list = &(graph->array[i]);
        adj_node = adj_list->head;
        while (adj_node != NULL) {
            adj_list->head = adj_node->next;
            free(adj_node);
            adj_node = adj_list->head;
        }
    }

    free(graph->array);
    free(graph);
}


//La fonction print_graph prend un pointeur vers un objet Graph et un tableau de Station
// et affiche toutes les connexions entre les stations avec leur poids (temps nécessaire 
//pour passer d'une station à l'autre), à partir de chaque station dans l'ordre du tableau de la station.
void print_graph(Graph* graph, Station stations[]) {
    for (int i = 0; i < graph->num_stations; i++) {
        printf("Station %d (%s): ", i, stations[i].name);
        AdjListNode* node = graph->array[i].head;
        while (node != NULL) {
            printf("%d (%d) ", stations[node->destination].id, node->weight);
            node = node->next;
        }
        printf("\n");
    }
}


// La fonction main est la fonction principale du programme et est exécutée en premier.
// Elle gère toutes les autres fonctions et organise les opérations du programme.
int main() {
    Station stations[MAX_STATIONS];
    Edge edges[MAX_EDGES];
    int num_stations, num_edges;
    Graph* graph;
    char start_station[MAX_LINE_LENGTH], end_station[MAX_LINE_LENGTH];
    int start_id, end_id;
    

    // La fonction read_stations lit les données des stations à partir du fichier CSV "Metro Paris Data - Stations.csv" et renvoie le nombre de stations lues.
    // La fonction read_edges lit les données des arêtes à partir du fichier CSV "Metro Paris Data - Aretes.csv" et renvoie le nombre d'arêtes lues.
    num_stations = read_stations(stations);
    if (num_stations < 0) {
        fprintf(stderr, "Error reading station data\n");
        return -1;
    }
    num_edges = read_edges(edges);
    if (num_edges < 0) {
        fprintf(stderr, "Error reading edge data\n");
        return -1;
    }
    

    // La fonction create_graph crée un graphe à partir des données des stations et des arêtes en initialisant les listes d'adjacence et en ajoutant des arêtes au graphe.
    graph = create_graph(num_stations, edges, num_edges);



    // La fonction display_stations affiche la liste des stations à l'utilisateur à partir des données des stations lues.
    display_stations(stations, num_stations);
    //print_graph(graph, stations); // print all stations and their connections

    // L'utilisateur est invité à entrer les stations de départ et d'arrivée à l'aide de la fonction fgets.
    // Utilise la fonction get_station_id pour obtenir les IDs des stations de départ et d'arrivée.
    // La fonction get_station_id renvoie l'ID de la station dont le nom est passé en argument.
    printf("Entrez le nom de la station de départ : ");
    fgets(start_station, MAX_LINE_LENGTH, stdin);
    start_station[strcspn(start_station, "\n")] = 0;
    start_id = get_station_id(stations, num_stations, start_station);
    while (start_id == -1) {
        printf("Nom de station non valide, entrez le nom de la station de départ :  ");
        fgets(start_station, MAX_LINE_LENGTH, stdin);
        start_station[strcspn(start_station, "\n")] = 0;
        start_id = get_station_id(stations, num_stations, start_station);
    }
    
    printf("Entrez le nom de la station d'arrivée : ");
    fgets(end_station, MAX_LINE_LENGTH, stdin);
    end_station[strcspn(end_station, "\n")] = 0;
    end_id = get_station_id(stations, num_stations, end_station);
    while (end_id == -1) {
        printf("Nom de station non valide, entrez le nom de la station d'arrivée : ");
        fgets(end_station, MAX_LINE_LENGTH, stdin);
        end_station[strcspn(end_station, "\n")] = 0;
        end_id = get_station_id(stations, num_stations, end_station);
    }

    // Appelle la fonction dijkstra pour trouver le chemin le plus court entre les stations de départ et d'arrivée en utilisant l'algorithme de Dijkstra, en passant le graphe, start_id, end_id et le tableau des stations comme arguments.
    dijkstra(graph, start_id , end_id , stations);

    // La fonction free_graph libère la mémoire allouée dynamiquement pour le graphe en libérant les listes d'adjacence et le graphe lui-même.
    free_graph(graph);
    
    return 0;
}
