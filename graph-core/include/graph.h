#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    NODE_SYSTEM,
    NODE_DIRECTORY,
    NODE_FILE,
    NODE_PROCESS
} NodeType;

typedef enum {
    EDGE_CONTAINS,
    EDGE_USES,
    EDGE_PARENT_OF
} EdgeType;

typedef struct GraphNode {
    uint64_t id;
    NodeType type;
    char *name;
} GraphNode;

typedef struct GraphEdge {
    uint64_t id;
    uint64_t source;
    uint64_t target;
    EdgeType type;
} GraphEdge;

typedef struct Graph {
    GraphNode **nodes;
    size_t node_count;
    size_t node_capacity;

    GraphEdge **edges;
    size_t edge_count;
    size_t edge_capacity;

    uint64_t next_node_id;
    uint64_t next_edge_id;
} Graph;

Graph *graph_create(void);
void graph_destroy(Graph *graph);

GraphNode *graph_add_node(
    Graph *graph,
    NodeType type,
    const char *name
);

GraphEdge *graph_add_edge(
    Graph *graph,
    uint64_t source,
    uint64_t target,
    EdgeType type
);

GraphNode *graph_find_node(
    Graph *graph,
    uint64_t id
);

#endif

