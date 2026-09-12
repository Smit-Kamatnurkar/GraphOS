# GraphFS

## 1. Overview

GraphFS is the filesystem interface of the initial GraphOS prototype.

It provides a conventional filesystem interface while representing filesystem objects and their relationships in the GraphOS graph.

The initial implementation uses FUSE 3.

The purpose of GraphFS is to demonstrate that filesystem operations can be represented through graph operations.

---

## 2. Architecture

The initial GraphFS architecture is:

```mermaid
flowchart TD
    USER[User / Application]
    FUSE[FUSE 3]
    GRAPHFS[GraphFS]
    CORE[Graph Core]
    GRAPH[(Graph)]

    USER -->|Filesystem Operations| FUSE
    FUSE --> GRAPHFS
    GRAPHFS --> CORE
    CORE --> GRAPH
```

GraphFS should use Graph Core for graph operations rather than maintaining a separate filesystem-specific graph implementation.

---

## 3. Filesystem Objects

The initial filesystem model contains:

- SYSTEM
- DIRECTORY
- FILE

These objects are represented as graph nodes.

```mermaid
graph TD
    SYSTEM[System]
    DIRECTORY[Directory]
    FILE[File]

    SYSTEM -->|CONTAINS| DIRECTORY
    DIRECTORY -->|CONTAINS| FILE
```

---

## 4. Directory Creation

When a user executes:

```text
mkdir projects
```

GraphFS should create a DIRECTORY node and establish a relationship with its parent.

```mermaid
graph TD
    PARENT[Parent Directory]
    PROJECTS[Directory: projects]

    PARENT -->|CONTAINS| PROJECTS
```

The filesystem directory therefore corresponds to a graph node.

---

## 5. File Creation

When a user executes:

```text
touch hello.txt
```

GraphFS should create a FILE node and connect it to its parent directory.

```mermaid
graph TD
    DIRECTORY[Directory]
    FILE[File: hello.txt]

    DIRECTORY -->|CONTAINS| FILE
```

---

## 6. Nested Directories

GraphFS must support relationships between directories.

For example:

```text
mkdir projects
mkdir projects/graphos
touch projects/graphos/README.txt
```

The graph can represent this as:

```mermaid
graph TD
    SYSTEM[System]
    PROJECTS[Directory: projects]
    GRAPHOS[Directory: graphos]
    README[File: README.txt]

    SYSTEM -->|CONTAINS| PROJECTS
    PROJECTS -->|CONTAINS| GRAPHOS
    GRAPHOS -->|CONTAINS| README
```

---

## 7. Path Resolution

GraphFS should resolve filesystem paths by traversing graph relationships.

For example:

```text
/projects/graphos/README.txt
```

can be resolved through:

```mermaid
graph TD
    SYSTEM[System]
    PROJECTS[projects]
    GRAPHOS[graphos]
    README[README.txt]

    SYSTEM -->|CONTAINS| PROJECTS
    PROJECTS -->|CONTAINS| GRAPHOS
    GRAPHOS -->|CONTAINS| README
```

The path components are therefore resolved through graph relationships rather than through an independent filesystem hierarchy.

---

## 8. Graph Authority

The long-term design requires the graph to be the authoritative representation of filesystem relationships.

GraphFS should therefore avoid treating a normal directory tree as a second independent source of truth.

The intended model is:

```mermaid
flowchart TD
    GRAPH[(Graph)]

    SYSTEM[SYSTEM Node]
    DIRECTORIES[DIRECTORY Nodes]
    FILES[FILE Nodes]

    GRAPH --> SYSTEM
    GRAPH --> DIRECTORIES
    GRAPH --> FILES
```

Relationships between these nodes are represented by graph edges.

---

## 9. FUSE Interface

GraphFS initially uses FUSE 3 to expose the graph-backed filesystem to Linux userspace.

The initial implementation should support the following operations:

### Lifecycle

- `init`
- `destroy`

### Reading

- `getattr`
- `readdir`
- `open`
- `read`

### Writing

- `mkdir`
- `create`
- `write`

### File Management

- `unlink`
- `rename`

Operations should be implemented incrementally rather than all at once.

---

## 10. Filesystem Operation Flow

A typical filesystem operation should follow this general flow:

```mermaid
sequenceDiagram
    participant User
    participant FUSE
    participant GraphFS
    participant GraphCore
    participant Graph

    User->>FUSE: Filesystem operation
    FUSE->>GraphFS: FUSE callback
    GraphFS->>GraphCore: Graph operation
    GraphCore->>Graph: Query or modify graph
    Graph-->>GraphCore: Result
    GraphCore-->>GraphFS: Result
    GraphFS-->>FUSE: Operation result
    FUSE-->>User: Filesystem result
```

This keeps the graph layer separate from the FUSE interface.

---

## 11. Read Operations

### `getattr`

`getattr` should determine whether a requested graph object exists and return the appropriate filesystem attributes.

The implementation must distinguish between:

- Files
- Directories
- Missing objects

---

### `readdir`

`readdir` should identify the children of a directory by querying the graph.

For example:

```mermaid
graph TD
    DIRECTORY[Directory: projects]
    FILE1[File: hello.txt]
    FILE2[File: test.txt]

    DIRECTORY -->|CONTAINS| FILE1
    DIRECTORY -->|CONTAINS| FILE2
```

A `readdir` operation should return:

```text
hello.txt
test.txt
```

based on the graph relationships.

---

## 12. Write Operations

### `mkdir`

Creates a DIRECTORY node and a `CONTAINS` edge.

### `create`

Creates a FILE node and a `CONTAINS` edge.

### `write`

Stores file data associated with the corresponding FILE node.

The exact storage mechanism may evolve during development.

---

## 13. File Data

The graph represents the existence and relationships of files.

File contents may be stored separately from graph metadata while remaining associated with the corresponding FILE node.

Conceptually:

```mermaid
flowchart TD
    FILE[FILE Node]
    METADATA[File Metadata]
    DATA[File Content]

    FILE --> METADATA
    FILE --> DATA
```

The graph remains authoritative for the existence and relationships of the file.

---

## 14. Deleting Files

When:

```text
rm hello.txt
```

is executed, GraphFS should:

1. Locate the corresponding FILE node.
2. Remove or update the relevant graph relationship.
3. Remove the file's stored data according to the persistence design.
4. Return the appropriate filesystem result.

The implementation must ensure that deleted files do not remain incorrectly reachable through the graph.

---

## 15. Renaming Files

When:

```text
mv hello.txt newname.txt
```

is executed, GraphFS should update the graph representation so that the FILE node is associated with the new name and, if necessary, a different parent directory.

The graph relationships must remain consistent after the operation.

---

## 16. Persistence

GraphFS must eventually persist graph state so that the graph survives process termination and remounting.

Initial storage may use:

```text
storage/
├── nodes.dat
└── edges.dat
```

The exact serialization format may evolve during implementation.

Runtime data should not be committed to the Git repository.

---

## 17. Mount Lifecycle

The intended lifecycle is:

```mermaid
flowchart TD
    START[Start GraphFS]
    LOAD[Load Graph]
    MOUNT[Mount FUSE Filesystem]
    OPERATE[Filesystem Operations]
    SAVE[Save Graph]
    UNMOUNT[Unmount]
    EXIT[Exit]

    START --> LOAD
    LOAD --> MOUNT
    MOUNT --> OPERATE
    OPERATE --> SAVE
    SAVE --> UNMOUNT
    UNMOUNT --> EXIT
```

The implementation should ensure that graph state is not lost during normal shutdown.

---

## 18. Initial Success Criteria

GraphFS is considered functional when a user can:

1. Start GraphFS.
2. Mount the filesystem.
3. Create a directory.
4. Create a file.
5. Write data to the file.
6. Read the data back.
7. List the directory.
8. Rename the file.
9. Delete the file.
10. Unmount GraphFS.
11. Restart GraphFS.
12. Recover persistent graph state.

---

## 19. Prototype Scope

The initial GraphFS implementation should remain small.

The first objective is to prove:

```mermaid
flowchart LR
    A[Filesystem Operation]
    B[Graph Operation]
    C[Graph State]

    A --> B
    B --> C
```

Once this relationship is stable, more advanced filesystem features can be added.

---

## 20. Future Development

Future versions may integrate GraphFS more deeply with Linux VFS and eventually reduce the distinction between GraphOS graph objects and native kernel resources.

Potential future areas include:

- Permissions
- Users
- Groups
- Mount relationships
- Devices
- Sockets
- Network resources
- Extended attributes
- Symbolic links
- File locking
- Memory mapping

These features are outside the initial prototype scope.
