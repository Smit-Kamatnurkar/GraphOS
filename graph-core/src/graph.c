#include "../include/graph.h"

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

Graph *graph_create(void)
{
    Graph *graph = malloc(sizeof(Graph));

    if (!graph)
        return NULL;

    graph->nodes =
        calloc(INITIAL_CAPACITY, sizeof(GraphNode *));

    graph->edges =
        calloc(INITIAL_CAPACITY, sizeof(GraphEdge *));

    if (!graph->nodes || !graph->edges) {
        free(graph->nodes);
        free(graph->edges);
        free(graph);
        return NULL;
    }

    graph->node_count = 0;
    graph->node_capacity = INITIAL_CAPACITY;

    graph->edge_count = 0;
    graph->edge_capacity = INITIAL_CAPACITY;

    graph->next_node_id = 1;
    graph->next_edge_id = 1;

    return graph;
}

void graph_destroy(Graph *graph)
{
    if (!graph)
        return;

    for (size_t i = 0; i < graph->node_count; i++) {
        free(graph->nodes[i]->name);
        free(graph->nodes[i]);
    }

    for (size_t i = 0; i < graph->edge_count; i++) {
        free(graph->edges[i]);
    }

    free(graph->nodes);
    free(graph->edges);
    free(graph);
}

static int grow_nodes(Graph *graph)
{
    size_t new_capacity =
        graph->node_capacity * 2;

    GraphNode **new_nodes =
        realloc(
            graph->nodes,
            new_capacity * sizeof(GraphNode *)
        );

    if (!new_nodes)
        return -1;

    graph->nodes = new_nodes;
    graph->node_capacity = new_capacity;

    return 0;
}

static int grow_edges(Graph *graph)
{
    size_t new_capacity =
        graph->edge_capacity * 2;

    GraphEdge **new_edges =
        realloc(
            graph->edges,
            new_capacity * sizeof(GraphEdge *)
        );

    if (!new_edges)
        return -1;

    graph->edges = new_edges;
    graph->edge_capacity = new_capacity;

    return 0;
}

GraphNode *graph_add_node(
    Graph *graph,
    NodeType type,
    const char *name
)
{
    if (!graph || !name)
        return NULL;

    if (graph->node_count >= graph->node_capacity) {
        if (grow_nodes(graph) != 0)
            return NULL;
    }

    GraphNode *node =
        malloc(sizeof(GraphNode));

    if (!node)
        return NULL;

    node->id = graph->next_node_id++;
    node->type = type;
    node->name = strdup(name);

    if (!node->name) {
        free(node);
        return NULL;
    }

    graph->nodes[graph->node_count++] = node;

    return node;
}

GraphEdge *graph_add_edge(
    Graph *graph,
    uint64_t source,
    uint64_t target,
    EdgeType type
)
{
    if (!graph)
        return NULL;

    if (graph->edge_count >= graph->edge_capacity) {
        if (grow_edges(graph) != 0)
            return NULL;
    }

    GraphEdge *edge =
        malloc(sizeof(GraphEdge));

    if (!edge)
        return NULL;

    edge->id = graph->next_edge_id++;
    edge->source = source;
    edge->target = target;
    edge->type = type;

    graph->edges[graph->edge_count++] = edge;

    return edge;
}

GraphNode *graph_find_node(
    Graph *graph,
    uint64_t id
)
{
    if (!graph)
        return NULL;

    for (size_t i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i]->id == id)
            return graph->nodes[i];
    }

    return NULL;
}
