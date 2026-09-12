#include "../include/graph.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

/*
 * Create a heap-allocated copy of a string.
 *
 * We use our own implementation instead of strdup()
 * so that the Graph Core does not depend on POSIX-specific
 * functions.
 */
static char *duplicate_string(const char *source)
{
    size_t length;
    char *copy;

    if (!source)
        return NULL;

    length = strlen(source);

    copy = malloc(length + 1);

    if (!copy)
        return NULL;

    memcpy(copy, source, length + 1);

    return copy;
}

/*
 * Validate a node type.
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
 * Validate an edge type.
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
 * Increase the node array capacity.
 */
static int grow_nodes(Graph *graph)
{
    size_t new_capacity;
    GraphNode **new_nodes;

    if (!graph)
        return -1;

    /*
     * Prevent integer overflow when doubling the capacity.
     */
    if (graph->node_capacity >
        SIZE_MAX / 2)
        return -1;

    new_capacity =
        graph->node_capacity * 2;

    /*
     * Prevent overflow in the allocation size.
     */
    if (new_capacity >
        SIZE_MAX / sizeof(GraphNode *))
        return -1;

    new_nodes =
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
 * Increase the edge array capacity.
 */
static int grow_edges(Graph *graph)
{
    size_t new_capacity;
    GraphEdge **new_edges;

    if (!graph)
        return -1;

    /*
     * Prevent integer overflow when doubling the capacity.
     */
    if (graph->edge_capacity >
        SIZE_MAX / 2)
        return -1;

    new_capacity =
        graph->edge_capacity * 2;

    /*
     * Prevent overflow in the allocation size.
     */
    if (new_capacity >
        SIZE_MAX / sizeof(GraphEdge *))
        return -1;

    new_edges =
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
 * Create an empty Graph.
 */
Graph *graph_create(void)
{
    Graph *graph;

    graph = malloc(sizeof(Graph));

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

    /*
     * If either allocation fails, clean up
     * everything that was successfully allocated.
     */
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
     * IDs start at 1.
     * ID 0 is therefore reserved as an invalid/non-existent ID.
     */
    graph->next_node_id = 1;
    graph->next_edge_id = 1;

    return graph;
}

/*
 * Destroy the entire Graph and release all memory.
 */
void graph_destroy(Graph *graph)
{
    size_t i;

    if (!graph)
        return;

    /*
     * Free every node and its name.
     */
    for (i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i]) {
            free(graph->nodes[i]->name);
            free(graph->nodes[i]);
        }
    }

    /*
     * Free every edge.
     */
    for (i = 0; i < graph->edge_count; i++) {
        free(graph->edges[i]);
    }

    /*
     * Finally free the arrays and Graph itself.
     */
    free(graph->nodes);
    free(graph->edges);
    free(graph);
}

/*
 * Add a new node to the Graph.
 */
GraphNode *graph_add_node(
    Graph *graph,
    NodeType type,
    const char *name
)
{
    GraphNode *node;
    char *name_copy;

    /*
     * Basic argument validation.
     */
    if (!graph || !name)
        return NULL;

    /*
     * Make sure the node type is valid.
     */
    if (!valid_node_type(type))
        return NULL;

    /*
     * Grow the node array if necessary.
     */
    if (graph->node_count >= graph->node_capacity) {
        if (grow_nodes(graph) != 0)
            return NULL;
    }

    /*
     * Allocate the node itself.
     */
    node = malloc(sizeof(GraphNode));

    if (!node)
        return NULL;

    /*
     * Copy the name.
     *
     * The Graph owns this copy and will free it
     * inside graph_destroy().
     */
    name_copy = duplicate_string(name);

    if (!name_copy) {
        free(node);
        return NULL;
    }

    /*
     * Only assign the ID after all allocations
     * have succeeded.
     */
    node->id = graph->next_node_id++;
    node->type = type;
    node->name = name_copy;

    /*
     * Store the node in the Graph.
     */
    graph->nodes[graph->node_count++] = node;

    return node;
}

/*
 * Add a new edge between two existing nodes.
 */
GraphEdge *graph_add_edge(
    Graph *graph,
    uint64_t source,
    uint64_t target,
    EdgeType type
)
{
    GraphEdge *edge;

    /*
     * Basic validation.
     */
    if (!graph)
        return NULL;

    /*
     * Make sure the edge type is valid.
     */
    if (!valid_edge_type(type))
        return NULL;

    /*
     * Node ID 0 is reserved as invalid.
     */
    if (source == 0 || target == 0)
        return NULL;

    /*
     * Make sure both endpoints actually exist.
     */
    if (!graph_find_node(graph, source))
        return NULL;

    if (!graph_find_node(graph, target))
        return NULL;

    /*
     * For the current Graph Core design,
     * self-edges are not allowed.
     */
    if (source == target)
        return NULL;

    /*
     * Grow the edge array if necessary.
     */
    if (graph->edge_count >= graph->edge_capacity) {
        if (grow_edges(graph) != 0)
            return NULL;
    }

    /*
     * Allocate the edge.
     */
    edge = malloc(sizeof(GraphEdge));

    if (!edge)
        return NULL;

    /*
     * Assign the edge data.
     */
    edge->id = graph->next_edge_id++;
    edge->source = source;
    edge->target = target;
    edge->type = type;

    /*
     * Store the edge.
     */
    graph->edges[graph->edge_count++] = edge;

    return edge;
}

/*
 * Find a node by its ID.
 *
 * Current implementation uses a simple linear search.
 * This is intentional for the first Graph Core version.
 */
GraphNode *graph_find_node(
    Graph *graph,
    uint64_t id
)
{
    size_t i;

    if (!graph)
        return NULL;

    /*
     * ID 0 is invalid.
     */
    if (id == 0)
        return NULL;

    for (i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i] &&
            graph->nodes[i]->id == id) {
            return graph->nodes[i];
        }
    }

    return NULL;
}
