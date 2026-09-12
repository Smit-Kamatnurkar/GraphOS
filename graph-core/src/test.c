#include "../include/graph.h"

#include <stdio.h>

int main(void)
{
    Graph *graph = graph_create();

    if (!graph) {
        printf("Failed to create graph\n");
        return 1;
    }

    GraphNode *system =
        graph_add_node(
            graph,
            NODE_SYSTEM,
            "GraphOS"
        );

    GraphNode *projects =
        graph_add_node(
            graph,
            NODE_DIRECTORY,
            "projects"
        );

    GraphNode *file =
        graph_add_node(
            graph,
            NODE_FILE,
            "hello.txt"
        );

    if (!system || !projects || !file) {
        printf("Failed to create nodes\n");
        graph_destroy(graph);
        return 1;
    }

    graph_add_edge(
        graph,
        system->id,
        projects->id,
        EDGE_CONTAINS
    );

    graph_add_edge(
        graph,
        projects->id,
        file->id,
        EDGE_CONTAINS
    );

    printf("GraphOS Graph Test\n\n");

    printf(
        "Node %lu: %s\n",
        system->id,
        system->name
    );

    printf(
        "Node %lu: %s\n",
        projects->id,
        projects->name
    );

    printf(
        "Node %lu: %s\n",
        file->id,
        file->name
    );

    printf(
        "\nNodes: %zu\n",
        graph->node_count
    );

    printf(
        "Edges: %zu\n",
        graph->edge_count
    );

    graph_destroy(graph);

    printf(
        "\nGraph test completed successfully.\n"
    );

    return 0;
}
