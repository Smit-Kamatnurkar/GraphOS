#include "../include/graph.h"
#include <stdio.h>
#include <assert.h>

int main(void)
{
    printf("Starting Graph Core API Tests...\n");

    Graph *graph = graph_create();
    assert(graph != NULL);

    /* 1. Test Node Creation */
    GraphNode *system = graph_add_node(graph, NODE_SYSTEM, "GraphOS");
    GraphNode *projects = graph_add_node(graph, NODE_DIRECTORY, "projects");
    GraphNode *file1 = graph_add_node(graph, NODE_FILE, "hello.txt");
    GraphNode *file2 = graph_add_node(graph, NODE_FILE, "world.txt");
    
    assert(system != NULL);
    assert(projects != NULL);
    assert(file1 != NULL);
    assert(file2 != NULL);
    assert(graph->node_count == 4);
    printf("[OK] Node creation\n");

    /* 2. Test Edge Creation */
    GraphEdge *e1 = graph_add_edge(graph, system->id, projects->id, EDGE_CONTAINS);
    GraphEdge *e2 = graph_add_edge(graph, projects->id, file1->id, EDGE_CONTAINS);
    GraphEdge *e3 = graph_add_edge(graph, projects->id, file2->id, EDGE_CONTAINS);
    
    assert(e1 != NULL);
    assert(e2 != NULL);
    assert(e3 != NULL);
    assert(graph->edge_count == 3);
    printf("[OK] Edge creation\n");

    /* 3. Test Find by Name */
    GraphNode *found = graph_find_node_by_name(graph, "hello.txt");
    assert(found != NULL);
    assert(found->id == file1->id);
    printf("[OK] Find node by name\n");

    /* 4. Test Find Parent */
    GraphNode *parent = graph_find_parent(graph, file1->id);
    assert(parent != NULL);
    assert(parent->id == projects->id);
    printf("[OK] Find parent\n");

    /* 5. Test Find Children */
    NodeList *children = graph_find_children(graph, projects->id);
    assert(children != NULL);
    assert(children->count == 2);
    /* The order of children isn't strictly guaranteed by API, but we know it's file1 and file2 */
    graph_node_list_free(children);
    printf("[OK] Find children\n");

    /* 6. Test Delete Edge */
    int res = graph_delete_edge(graph, e3->id);
    assert(res == 0);
    assert(graph->edge_count == 2);
    /* Verify edge is gone */
    NodeList *children_after = graph_find_children(graph, projects->id);
    assert(children_after != NULL);
    assert(children_after->count == 1);
    graph_node_list_free(children_after);
    printf("[OK] Delete edge\n");

    /* 7. Test Delete Node (with cascading edge deletion) */
    /* Deleting projects directory should cascade and delete edges connected to it */
    res = graph_delete_node(graph, projects->id);
    assert(res == 0);
    assert(graph->node_count == 3);
    /* Should have removed e1 and e2 since they touch 'projects' */
    assert(graph->edge_count == 0);
    printf("[OK] Delete node (cascading edge removal)\n");

    /* Clean up */
    graph_destroy(graph);
    printf("\nAll Graph Core tests passed successfully!\n");

    return 0;
}
