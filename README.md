# GraphOS

> **A graph-native operating system built around relationships between system resources.**

GraphOS is an experimental operating-system project exploring a different way of representing and interacting with a computer system: **as a graph of resources and relationships rather than merely a collection of isolated processes, files, devices, and services.**

The project combines a graph-based system model with Linux kernel infrastructure, a graph-aware filesystem interface, system monitoring, and OS-level services.

The long-term goal is to build a **bootable GraphOS environment** that can run inside a virtual machine and demonstrate how graph-native operating-system concepts can be integrated with a conventional Linux kernel.

---

## Table of Contents

- [Overview](#overview)
- [Motivation](#motivation)
- [Core Idea](#core-idea)
- [Architecture](#architecture)
- [System Components](#system-components)
- [Resource Graph](#resource-graph)
- [GraphFS](#graphfs)
- [Process Monitor](#process-monitor)
- [graphctl](#graphctl)
- [Linux Integration](#linux-integration)
- [Graph-aware OS Services](#graph-aware-os-services)
- [Bootable GraphOS](#bootable-graphos)
- [Execution Environment](#execution-environment)
- [Repository Structure](#repository-structure)
- [Technology Stack](#technology-stack)
- [Development Roadmap](#development-roadmap)
- [Current Status](#current-status)
- [Design Principles](#design-principles)
- [Future Work](#future-work)
- [Contributing](#contributing)
- [License](#license)

---

# Overview

Traditional operating systems expose system resources through multiple separate abstractions:

- Processes through process-management interfaces
- Files through filesystems
- Network connections through networking APIs
- Devices through device files and kernel subsystems
- Services through service managers
- Dependencies and relationships through a mixture of system interfaces and implicit metadata

GraphOS explores an alternative approach:

> **What if the operating system maintained a unified graph representing the resources of the system and the relationships between them?**

In GraphOS, entities such as processes, files, devices, sockets, and services can be represented as **nodes**, while interactions and dependencies can be represented as **edges**.

For example:

```mermaid
flowchart TD
    P["Process"] -->|opens| F["File"]
    P -->|accesses| D["Device"]
    P -->|creates| N["Network Socket"]
    P -->|depends on| S["System Service"]

### The `graphctl` Command Reference
`graphctl` is the custom command-line interface for GraphOS. Instead of relying on legacy POSIX commands (like `mkdir` or `ln`), it translates user commands directly into native graph operations via a virtual kernel control channel (`/.graph_cmd`).

* `spawn-dir <path>`: Creates a new DIRECTORY node.
* `spawn-file <path> <content>`: Creates a new FILE node with data buffer.
* `read-node <path>`: Retrieves the data buffer from a FILE node.
* `draw-edge <src> <dest>`: Draws a new CONTAINS edge to an existing node (enables native graph hardlinking without duplicating data).
* `sever-edge <path>`: Deletes a single CONTAINS edge. Keeps the node alive if it still has incoming edges from other nodes.
* `sever-all-edges <path>`: Global force-delete. Sends a SEVER_ALL signal through the control channel, destroying the node entirely and cascading to destroy ALL edges pointing to it anywhere in the OS.
* `destroy-node <path>`: Destroys an empty DIRECTORY node.
* `inspect-graph`: Dumps the raw graph state (Nodes and Edges) directly from memory to visualize the true architecture.
