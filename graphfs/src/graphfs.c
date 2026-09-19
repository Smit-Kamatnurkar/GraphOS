#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <time.h>
#include "../../graph-core/include/graph.h"

// --------------------------------------------------------
// In-memory file storage
// --------------------------------------------------------
typedef struct {
    uint64_t node_id;
    char *data;
    size_t size;
} FileContent;

#define MAX_FILES 1024
static FileContent file_storage[MAX_FILES];
static size_t file_storage_count = 0;

static FileContent* get_file_content(uint64_t node_id) {
    for (size_t i = 0; i < file_storage_count; i++) {
        if (file_storage[i].node_id == node_id) return &file_storage[i];
    }
    return NULL;
}

static int add_file_content(uint64_t node_id) {
    if (file_storage_count >= MAX_FILES) return -1;
    file_storage[file_storage_count].node_id = node_id;
    file_storage[file_storage_count].data = NULL;
    file_storage[file_storage_count].size = 0;
    file_storage_count++;
    return 0;
}

static void remove_file_content(uint64_t node_id) {
    for (size_t i = 0; i < file_storage_count; i++) {
        if (file_storage[i].node_id == node_id) {
            free(file_storage[i].data);
            file_storage[i] = file_storage[file_storage_count - 1];
            file_storage_count--;
            return;
        }
    }
}

// --------------------------------------------------------
// GraphFS Context and Path Resolution
// --------------------------------------------------------
typedef struct {
    Graph *graph;
    GraphNode *root_node;
} GraphFSContext;

static GraphNode* resolve_path(const char *path) {
    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    Graph *g = ctx->graph;
    GraphNode *current = ctx->root_node;

    if (strcmp(path, "/") == 0) return current;

    char *path_copy = strdup(path);
    char *token = strtok(path_copy, "/");

    while (token != NULL) {
        NodeList *children = graph_find_children(g, current->id);
        if (!children) { free(path_copy); return NULL; }

        GraphNode *next = NULL;
        for (size_t i = 0; i < children->count; i++) {
            if (strcmp(children->items[i]->name, token) == 0) {
                next = children->items[i];
                break;
            }
        }
        graph_node_list_free(children);

        if (!next) { free(path_copy); return NULL; }
        current = next;
        token = strtok(NULL, "/");
    }
    free(path_copy);
    return current;
}

static int get_parent_and_name(const char *path, GraphNode **parent, char **name) {
    char *path_copy = strdup(path);
    char *last_slash = strrchr(path_copy, '/');
    if (!last_slash) { free(path_copy); return -EINVAL; }

    *last_slash = '\0';
    const char *parent_path = (path_copy[0] == '\0') ? "/" : path_copy;
    *parent = resolve_path(parent_path);

    if (!*parent) { free(path_copy); return -ENOENT; }
    *name = strdup(last_slash + 1);
    free(path_copy);
    return 0;
}

// --------------------------------------------------------
// FUSE Operations
// --------------------------------------------------------

static void* gfs_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    (void) conn;
    cfg->kernel_cache = 0; // Disable caching for accurate demo
    printf("[GraphFS] Initializing GraphFS\n");
    
    GraphFSContext *ctx = malloc(sizeof(GraphFSContext));
    ctx->graph = graph_create();
    ctx->root_node = graph_add_node(ctx->graph, NODE_SYSTEM, "root");
    
    printf("[GraphFS] Initialized graph with root SYSTEM node (id=%lu)\n", (unsigned long)ctx->root_node->id);
    return ctx;
}

static void gfs_destroy(void *private_data) {
    printf("[GraphFS] Shutting down GraphFS\n");
    GraphFSContext *ctx = (GraphFSContext *)private_data;
    for(size_t i = 0; i < file_storage_count; i++) {
        free(file_storage[i].data);
    }
    graph_destroy(ctx->graph);
    free(ctx);
}

static int gfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/.graph") == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 4096;
        stbuf->st_uid = fuse_get_context()->uid;
        stbuf->st_gid = fuse_get_context()->gid;
        stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);
        return 0;
    }

    GraphNode *node = resolve_path(path);
    if (!node) return -ENOENT;

    if (node->type == NODE_SYSTEM || node->type == NODE_DIRECTORY) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        printf("[GraphFS] getattr: resolved \"%s\" -> DIRECTORY node (id=%lu)\n", path, (unsigned long)node->id);
    } else if (node->type == NODE_FILE) {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        FileContent *fc = get_file_content(node->id);
        stbuf->st_size = fc ? fc->size : 0;
        printf("[GraphFS] getattr: resolved \"%s\" -> FILE node (id=%lu)\n", path, (unsigned long)node->id);
    } else {
        return -ENOENT;
    }
    
    stbuf->st_uid = fuse_get_context()->uid;
    stbuf->st_gid = fuse_get_context()->gid;
    stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);
    return 0;
}

static int gfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    (void) offset; (void) fi; (void) flags;
    GraphNode *node = resolve_path(path);
    if (!node) return -ENOENT;
    if (node->type != NODE_SYSTEM && node->type != NODE_DIRECTORY) return -ENOTDIR;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    if (strcmp(path, "/") == 0) {
        filler(buf, ".graph", NULL, 0, 0);
    }

    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    NodeList *children = graph_find_children(ctx->graph, node->id);
    if (children) {
        printf("[GraphFS] readdir: \"%s\" has %zu children\n", path, children->count);
        for (size_t i = 0; i < children->count; i++) {
            filler(buf, children->items[i]->name, NULL, 0, 0);
        }
        graph_node_list_free(children);
    } else {
        printf("[GraphFS] readdir: \"%s\" has 0 children\n", path);
    }
    return 0;
}

static int gfs_mkdir(const char *path, mode_t mode) {
    (void) mode;
    GraphNode *parent;
    char *name;
    int res = get_parent_and_name(path, &parent, &name);
    if (res != 0) return res;

    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    GraphNode *new_node = graph_add_node(ctx->graph, NODE_DIRECTORY, name);
    graph_add_edge(ctx->graph, parent->id, new_node->id, EDGE_CONTAINS);

    printf("[GraphFS] mkdir: Created DIRECTORY node \"%s\" (id=%lu)\n", name, (unsigned long)new_node->id);
    printf("[GraphFS] mkdir: Added CONTAINS edge (%lu -> %lu)\n", (unsigned long)parent->id, (unsigned long)new_node->id);
    free(name);
    return 0;
}

static int gfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void) mode; (void) fi;
    GraphNode *parent;
    char *name;
    int res = get_parent_and_name(path, &parent, &name);
    if (res != 0) return res;

    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    GraphNode *new_node = graph_add_node(ctx->graph, NODE_FILE, name);
    graph_add_edge(ctx->graph, parent->id, new_node->id, EDGE_CONTAINS);
    add_file_content(new_node->id);

    printf("[GraphFS] create: Created FILE node \"%s\" (id=%lu)\n", name, (unsigned long)new_node->id);
    printf("[GraphFS] create: Added CONTAINS edge (%lu -> %lu)\n", (unsigned long)parent->id, (unsigned long)new_node->id);
    free(name);
    return 0;
}

static int gfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    GraphNode *node = resolve_path(path);
    if (!node) return -ENOENT;
    if (node->type != NODE_FILE) return -EISDIR;

    FileContent *fc = get_file_content(node->id);
    if (!fc) return -ENOENT;

    if (offset + size > fc->size) {
        char *new_data = realloc(fc->data, offset + size);
        if (!new_data) return -ENOMEM;
        fc->data = new_data;
        fc->size = offset + size;
    }
    memcpy(fc->data + offset, buf, size);
    printf("[GraphFS] write: Wrote %zu bytes to node %lu\n", size, (unsigned long)node->id);
    return size;
}

static int gfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    
    if (strcmp(path, "/.graph") == 0) {
        GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
        char temp[4096] = {0};
        int written = 0;
        
        written += snprintf(temp + written, sizeof(temp) - written, "=== GraphOS Live Graph State ===\n\n[NODES]\n");
        for (size_t i = 0; i < ctx->graph->node_count; i++) {
            if (ctx->graph->nodes[i]) {
                GraphNode *n = ctx->graph->nodes[i];
                const char *type = (n->type == NODE_SYSTEM) ? "SYSTEM" : ((n->type == NODE_DIRECTORY) ? "DIRECTORY" : "FILE");
                written += snprintf(temp + written, sizeof(temp) - written, "  (%lu) [%s] %s\n", (unsigned long)n->id, type, n->name);
            }
        }
        
        written += snprintf(temp + written, sizeof(temp) - written, "\n[EDGES]\n");
        for (size_t i = 0; i < ctx->graph->edge_count; i++) {
            if (ctx->graph->edges[i]) {
                GraphEdge *e = ctx->graph->edges[i];
                const char *edge_type = (e->type == EDGE_CONTAINS) ? "CONTAINS" : ((e->type == EDGE_USES) ? "USES" : "PARENT");
                written += snprintf(temp + written, sizeof(temp) - written, "  %lu ---> %lu [%s]\n", (unsigned long)e->source, (unsigned long)e->target, edge_type);
            }
        }
        
        if ((size_t)offset >= (size_t)written) return 0;
        if (offset + size > (size_t)written) size = written - offset;
        memcpy(buf, temp + offset, size);
        return size;
    }

    GraphNode *node = resolve_path(path);
    if (!node) return -ENOENT;
    if (node->type != NODE_FILE) return -EISDIR;

    FileContent *fc = get_file_content(node->id);
    if (!fc) return -ENOENT;

    if ((size_t)offset >= fc->size) return 0;
    if (offset + size > fc->size) size = fc->size - offset;

    memcpy(buf, fc->data + offset, size);
    printf("[GraphFS] read: Read %zu bytes from node %lu\n", size, (unsigned long)node->id);
    return size;
}

static int gfs_unlink(const char *path) {
    GraphNode *node = resolve_path(path);
    if (!node) return -ENOENT;
    if (node->type != NODE_FILE) return -EISDIR;

    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    uint64_t id = node->id;
    
    remove_file_content(id);
    graph_delete_node(ctx->graph, id); // cascades edge deletion
    
    printf("[GraphFS] unlink: Deleted FILE node \"%s\" (id=%lu)\n", path, (unsigned long)id);
    return 0;
}

static int gfs_rmdir(const char *path) {
    GraphNode *node = resolve_path(path);
    if (!node) return -ENOENT;
    if (node->type != NODE_DIRECTORY) return -ENOTDIR;

    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    NodeList *children = graph_find_children(ctx->graph, node->id);
    if (children && children->count > 0) {
        graph_node_list_free(children);
        return -ENOTEMPTY;
    }
    if (children) graph_node_list_free(children);

    uint64_t id = node->id;
    graph_delete_node(ctx->graph, id);
    
    printf("[GraphFS] rmdir: Deleted DIRECTORY node \"%s\" (id=%lu)\n", path, (unsigned long)id);
    return 0;
}

static int gfs_rename(const char *from, const char *to, unsigned int flags) {
    if (flags) return -EINVAL; // no flag support

    GraphNode *node = resolve_path(from);
    if (!node) return -ENOENT;

    GraphNode *target = resolve_path(to);
    if (target) {
        if (target->type == NODE_DIRECTORY) {
            int r = gfs_rmdir(to);
            if (r != 0) return r;
        } else {
            int r = gfs_unlink(to);
            if (r != 0) return r;
        }
    }

    GraphNode *new_parent;
    char *new_name;
    int res = get_parent_and_name(to, &new_parent, &new_name);
    if (res != 0) return res;

    GraphFSContext *ctx = (GraphFSContext *)fuse_get_context()->private_data;
    GraphNode *old_parent = graph_find_parent(ctx->graph, node->id);

    free(node->name);
    node->name = strdup(new_name);

    if (old_parent && old_parent->id != new_parent->id) {
        for (size_t i = 0; i < ctx->graph->edge_count; i++) {
            if (ctx->graph->edges[i] && ctx->graph->edges[i]->source == old_parent->id && ctx->graph->edges[i]->target == node->id) {
                graph_delete_edge(ctx->graph, ctx->graph->edges[i]->id);
                break;
            }
        }
        graph_add_edge(ctx->graph, new_parent->id, node->id, EDGE_CONTAINS);
        printf("[GraphFS] rename: Reparented node %lu\n", (unsigned long)node->id);
    }

    printf("[GraphFS] rename: Renamed node %lu to \"%s\"\n", (unsigned long)node->id, new_name);
    free(new_name);
    return 0;
}

static int gfs_utimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi) {
    (void) path; (void) tv; (void) fi;
    return 0; // Dummy implementation so `touch` doesn't throw "Function not implemented"
}

static const struct fuse_operations gfs_oper = {
    .init       = gfs_init,
    .destroy    = gfs_destroy,
    .getattr    = gfs_getattr,
    .readdir    = gfs_readdir,
    .mkdir      = gfs_mkdir,
    .create     = gfs_create,
    .write      = gfs_write,
    .read       = gfs_read,
    .unlink     = gfs_unlink,
    .rmdir      = gfs_rmdir,
    .rename     = gfs_rename,
    .utimens    = gfs_utimens,
};

int main(int argc, char *argv[]) {
    printf("[GraphFS] Starting...\n");
    return fuse_main(argc, argv, &gfs_oper, NULL);
}
