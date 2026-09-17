#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>
#include <stdint.h>

/*
 * Node types supported by the Graph Core.
 *
 * SYSTEM      The root node representing the operating system.
 * DIRECTORY   A directory in the filesystem.
 * FILE        A file in the filesystem.
 * PROCESS     A running process.
 */
typedef enum {
    NODE_SYSTEM,
    NODE_DIRECTORY,
    NODE_FILE,
    NODE_PROCESS
} NodeType;

/*
 * Edge types supported by the Graph Core.
 *
 * CONTAINS    Containment relationship (e.g. directory contains file).
 * USES        Resource usage (e.g. process uses file).
 * PARENT_OF   Parent-child relationship (e.g. process hierarchy).
 */
typedef enum {
    EDGE_CONTAINS,
    EDGE_USES,
    EDGE_PARENT_OF
} EdgeType;

/*
 * A node in the graph.
 *
 * Each node has a unique non-zero ID, a type, and a
 * human-readable name.  The name is owned by the Graph
 * and freed when the node is destroyed.
 */
typedef struct {
    uint64_t  id;
    NodeType  type;
    char     *name;
} GraphNode;

/*
 * A directed edge between two nodes.
 *
 * Each edge has a unique non-zero ID, a source node,
 * a target node, and a type that describes the
 * relationship.  Self-edges are not permitted.
 */
typedef struct {
    uint64_t  id;
    uint64_t  source;
    uint64_t  target;
    EdgeType  type;
} GraphEdge;

/*
 * The top-level graph container.
 *
 * Stores dynamically growing arrays of node and edge
 * pointers, along with bookkeeping for capacity and
 * auto-incrementing IDs (starting at 1; 0 is reserved
 * as the invalid/non-existent sentinel).
 */
typedef struct {
    GraphNode **nodes;
    size_t      node_count;
    size_t      node_capacity;
    uint64_t    next_node_id;

    GraphEdge **edges;
    size_t      edge_count;
    size_t      edge_capacity;
    uint64_t    next_edge_id;
} Graph;

/*
 * Create an empty graph.
 * Returns NULL on allocation failure.
 */
Graph *graph_create(void);

/*
 * Destroy the graph and free all associated memory
 * (nodes, edges, names, and the graph itself).
 * Safe to call with NULL.
 */
void graph_destroy(Graph *graph);

/*
 * Add a node with the given type and name.
 * The graph takes ownership of a copy of the name string.
 * Returns the new node, or NULL on failure.
 */
GraphNode *graph_add_node(
    Graph      *graph,
    NodeType    type,
    const char *name
);

/*
 * Add a directed edge between two existing nodes.
 * Both source and target must be valid non-zero IDs
 * of nodes already in the graph, and must differ.
 * Returns the new edge, or NULL on failure.
 */
GraphEdge *graph_add_edge(
    Graph    *graph,
    uint64_t  source,
    uint64_t  target,
    EdgeType  type
);

/*
 * Find a node by its ID.
 * Returns the node, or NULL if not found.
 */
GraphNode *graph_find_node(
    Graph    *graph,
    uint64_t  id
);

/*
 * Delete a node by its ID.
 *
 * Also removes every edge that references the node
 * (as source or target).  Returns 0 on success, -1 on
 * failure (NULL graph, invalid ID, node not found).
 */
int graph_delete_node(
    Graph    *graph,
    uint64_t  id
);

/*
 * Delete an edge by its ID.
 * Returns 0 on success, -1 on failure.
 */
int graph_delete_edge(
    Graph    *graph,
    uint64_t  id
);

/*
 * A caller-owned list of node pointers returned by
 * query functions.
 *
 * The pointers inside 'items' still belong to the
 * Graph — the caller must free only the NodeList
 * itself via graph_node_list_free().
 */
typedef struct {
    GraphNode **items;
    size_t      count;
} NodeList;

/*
 * Free a NodeList returned by a query function.
 * Does NOT free the nodes themselves.
 * Safe to call with NULL.
 */
void graph_node_list_free(NodeList *list);

/*
 * Return every node that is the target of an edge
 * whose source is the given node ID.
 *
 * Caller must free the result with graph_node_list_free().
 * Returns NULL on failure.
 */
NodeList *graph_find_children(
    Graph    *graph,
    uint64_t  id
);

/*
 * Return the first node found that is the source of an
 * edge whose target is the given node ID.
 *
 * Returns the parent node, or NULL if none exists.
 */
GraphNode *graph_find_parent(
    Graph    *graph,
    uint64_t  id
);

/*
 * Find the first node whose name matches the given
 * string (exact, case-sensitive match).
 *
 * Returns the node, or NULL if not found.
 */
GraphNode *graph_find_node_by_name(
    Graph      *graph,
    const char *name
);

#endif /* GRAPH_H */
