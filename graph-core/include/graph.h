#include "../include/graph.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define INITIAL_CAPACITY 16

/*
 * Duplicate a string without relying on POSIX strdup().
 * The returned string is owned by the caller.
 */
static char *duplicate_string(const char *source)
{
    if (!source)
        return NULL;

    size_t length = strlen(source);

    char *copy = malloc(length + 1);

    if (!copy)
        return NULL;

    memcpy(copy, source, length + 1);

    return copy;
}

/*
 * Validate a NodeType.
 */
static int valid_node_type(NodeType type)
{
    switch (type) {
        case NODE_SYSTEM:
        case NODE_DIRECTORY:
        case NODE_FILE:
        case NODE_PROCESS:
            return 1;

        default:
            return 0;
    }
}

/*
 * Validate an EdgeType.
 */
static int valid_edge_type(EdgeType type)
{
    switch (type) {
        case EDGE_CONTAINS:
        case EDGE_USES:
        case EDGE_PARENT_OF:
            return 1;

        default:
            return 0;
    }
}

/*
 * Grow the node pointer array.
 */
static int grow_nodes(Graph *graph)
{
    if (!graph)
        return -1;

    /*
     * Prevent integer overflow when doubling capacity.
     */
    if (graph->node_capacity >
        SIZE_MAX / 2 / sizeof(GraphNode *))
        return -1;

    size_t new_capacity = graph->node_capacity * 2;

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

/*
 * Grow the edge pointer array.
 */
static int grow_edges(Graph *graph)
{
    if (!graph)
        return -1;

    /*
     * Prevent integer overflow when doubling capacity.
     */
    if (graph->edge_capacity >
        SIZE_MAX / 2 / sizeof(GraphEdge *))
        return -1;

    size_t new_capacity = graph->edge_capacity * 2;

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

/*
 * Create an empty graph.
 */
Graph *graph_create(void)
{
    Graph *graph = malloc(sizeof(Graph));

    if (!graph)
        return NULL;

    graph->nodes =
        calloc(
            INITIAL_CAPACITY,
            sizeof(GraphNode *)
        );

    graph->edges =
        calloc(
            INITIAL_CAPACITY,
            sizeof(GraphEdge *)
        );

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

    /*
     * IDs start at
