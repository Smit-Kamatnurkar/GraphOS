# GraphOS Architecture

## 1. Overview

GraphOS is an experimental operating system built around a graph-based architecture.

The central design principle of GraphOS is that operating-system objects and the relationships between those objects should be represented as a graph.

Files, directories, processes, resources, devices, services, and applications can be represented as nodes, while relationships between those objects are represented as edges.

The graph is intended to become a fundamental system abstraction rather than simply an additional layer of metadata.

---

## 2. Core Design Principle

Traditional operating systems expose different abstractions for different resources.

For example:

- Files are accessed through filesystems.
- Processes are managed by process-management mechanisms.
- Devices are exposed through device interfaces.
- Network resources are managed through networking subsystems.
- Services communicate through separate IPC mechanisms.

GraphOS aims to connect these concepts through a common graph model.

### GraphOS System Model

```mermaid
graph TD
    SYSTEM[System]

    DIRECTORY[Directory]
    FILE[File]
    PROCESS[Process]
    DEVICE[Device]
    SERVICE[Service]

    SYSTEM -->|CONTAINS| DIRECTORY
    DIRECTORY -->|CONTAINS| FILE

    PROCESS -->|USES| FILE
    PROCESS -->|USES| DEVICE

    SERVICE -->|USES| DEVICE
    PROCESS -->|USES| SERVICE
