---
name: GraphOS Terminal Environment
colors:
  surface: '#0f131d'
  surface-dim: '#0f131d'
  surface-bright: '#343944'
  surface-container-lowest: '#090e17'
  surface-container-low: '#171c25'
  surface-container: '#1b2029'
  surface-container-high: '#252a34'
  surface-container-highest: '#30353f'
  on-surface: '#dee2f0'
  on-surface-variant: '#bcc9cd'
  inverse-surface: '#dee2f0'
  inverse-on-surface: '#2c303b'
  outline: '#869397'
  outline-variant: '#3d494c'
  surface-tint: '#4cd7f6'
  primary: '#4cd7f6'
  on-primary: '#003640'
  primary-container: '#06b6d4'
  on-primary-container: '#00424f'
  inverse-primary: '#00687a'
  secondary: '#4edea3'
  on-secondary: '#003824'
  secondary-container: '#00a572'
  on-secondary-container: '#00311f'
  tertiary: '#ddb7ff'
  on-tertiary: '#490080'
  tertiary-container: '#c78dff'
  on-tertiary-container: '#5a009b'
  error: '#ffb4ab'
  on-error: '#690005'
  error-container: '#93000a'
  on-error-container: '#ffdad6'
  primary-fixed: '#acedff'
  primary-fixed-dim: '#4cd7f6'
  on-primary-fixed: '#001f26'
  on-primary-fixed-variant: '#004e5c'
  secondary-fixed: '#6ffbbe'
  secondary-fixed-dim: '#4edea3'
  on-secondary-fixed: '#002113'
  on-secondary-fixed-variant: '#005236'
  tertiary-fixed: '#f0dbff'
  tertiary-fixed-dim: '#ddb7ff'
  on-tertiary-fixed: '#2c0051'
  on-tertiary-fixed-variant: '#6900b3'
  background: '#0f131d'
  on-background: '#dee2f0'
  surface-variant: '#30353f'
typography:
  headline-lg:
    fontFamily: Inter
    fontSize: 1.25rem
    fontWeight: '600'
    lineHeight: 1.75rem
    letterSpacing: -0.015em
  headline-md:
    fontFamily: Inter
    fontSize: 1rem
    fontWeight: '600'
    lineHeight: 1.5rem
    letterSpacing: -0.01em
  headline-sm:
    fontFamily: Inter
    fontSize: 0.875rem
    fontWeight: '600'
    lineHeight: 1.25rem
    letterSpacing: 0em
  body-lg:
    fontFamily: JetBrains Mono
    fontSize: 0.9375rem
    fontWeight: '400'
    lineHeight: 1.5rem
    letterSpacing: 0em
  body-md:
    fontFamily: JetBrains Mono
    fontSize: 0.8125rem
    fontWeight: '400'
    lineHeight: 1.375rem
    letterSpacing: 0em
  body-sm:
    fontFamily: JetBrains Mono
    fontSize: 0.75rem
    fontWeight: '400'
    lineHeight: 1.125rem
    letterSpacing: 0em
  label-md:
    fontFamily: Inter
    fontSize: 0.75rem
    fontWeight: '500'
    lineHeight: 1rem
    letterSpacing: 0.02em
  label-sm:
    fontFamily: Inter
    fontSize: 0.6875rem
    fontWeight: '600'
    lineHeight: 0.875rem
    letterSpacing: 0.04em
  code-dense:
    fontFamily: JetBrains Mono
    fontSize: 0.6875rem
    fontWeight: '500'
    lineHeight: 1rem
    letterSpacing: 0em
rounded:
  sm: 0.125rem
  DEFAULT: 0.25rem
  md: 0.375rem
  lg: 0.5rem
  xl: 0.75rem
  full: 9999px
spacing:
  gutter: 0.5rem
  margin: 0.75rem
  space-xs: 0.125rem
  space-sm: 0.25rem
  space-md: 0.5rem
  space-lg: 0.75rem
  space-xl: 1rem
---

## Brand & Style

This design system targets systems engineers, cloud architects, and advanced developers who require rapid, context-rich terminal execution without context-switching overhead. The aesthetic is engineered around high information density, structural rigor, and instant readability. It explicitly rejects superficial science-fiction tropes, excessive neon glows, decorative transparency, and faux-physical skeuomorphism.

The interface evokes precise technical mastery, deep stability, and quiet operational authority. Surfaces feel cold, architectural, and deliberate—constructed from low-reflectance midnight charcoals and deep navies. Typography and semantic cues work in tight synchronization: every pixel, dividing hairline, and color swatch represents operational status, execution depth, or dependency relationships.

Key aesthetic pillars:
- **Utilitarian Rigor:** Zero decorative padding; UI chrome recedes completely, elevating terminal output and dependency graphs.
- **Semantic Determinism:** Colors possess immutable functional definitions (Cyan = graph intelligence/commands; Emerald = operational integrity; Violet = relational ontology; Amber = threshold warnings).
- **Subtle Precision:** Contrast is achieved through calibrated tone differentials and razor-thin borders rather than elevated drop shadows or blurs.

## Colors

The color palette is built for long working sessions under focused developer conditions. The visual field stays neutral and deep, allowing semantic tokens to signal execution states, dependency linkages, and error boundaries with zero visual ambiguity.

### Surface Palette
- `canvas-base`: `#0a0f18` — The primary root viewport and terminal backplane.
- `surface-panel`: `#0f172a` — Sidebar panes, split panes, and integrated graph trees.
- `surface-elevated`: `#131d31` — Active prompt bands, floating diagnostic overlays, and context drawers.
- `surface-hover`: `#1e293b` — Subtle interactive element states, active row selections, and command hints.

### Borders & Dividing Lines
- `border-subtle`: `#1e293b` — Default dividers between panes, tabs, and status regions.
- `border-strong`: `#334155` — Active tab highlight, focused split-pane border, command prompt focus boundary.

### Semantic & Graph Tokens
- `accent-cyan` (`#06b6d4`, highlight `#22d3ee`): Core project badge tokens, command strings, graph nodes representing primary services/binaries, and cursor emphasis.
- `accent-emerald` (`#10b981`, highlight `#34d399`): Execution success, `READY` daemon states, healthy health checks, and verified build signatures.
- `accent-violet` (`#a855f7`, highlight `#c084fc`): Directed graph relationship edges, semantic AST interfaces, package dependency lineage, and schema metadata.
- `accent-amber` (`#f59e0b`, highlight `#fbbf24`): Degraded process states, compiler warnings, uncommitted graph drift, and resource thresholds.
- `accent-rose` (`#f43f5e`, highlight `#fb7185`): Critical stderr outputs, broken dependencies, panic traces, and cycle collision alerts.

### Text Levels
- `text-primary`: `#f8fafc` — Primary commands, terminal stdout, active panel headers.
- `text-secondary`: `#94a3b8` — Flags, metadata, timestamp watermarks, inactive tabs.
- `text-muted`: `#64748b` — Line numbers, edge directional arrows, prompt decorators.

## Typography

The type system implements a strict dual-engine strategy: **Inter** manages high-level orchestration, application chrome, system menus, status bars, and window headers, while **JetBrains Mono** governs the core computing canvas—terminal input/output, interactive AST trees, node labels, log streams, and inline parameter matrices.

Key guidelines:
- **Monospace Purity:** Terminal output must retain strict tabular alignment. Never substitute proportional fonts inside log outputs or graph node identifiers.
- **Ligatures:** Standard programming ligatures (`->`, `!=`, `===`) are enabled in code representations, but relationship arrows in graph visualizers use dedicated geometric unicode or SVG tokens (`→`, `↳`) to preserve precise character cell widths.
- **Dense Hierarchy:** Small, legible scales down to `0.6875rem` (11px) are deployed to display Git SHAs, memory offsets, and container process IDs without line wrapping.

## Layout & Spacing

The layout model is governed by a **tiling split-pane architecture** rather than conventional marketing grids. Content scales from edge to edge with zero dead canvas space. Layout adjustments prioritize terminal line lengths, side-by-side terminal splits, and high-density dependency inspector sidebars.

### Spatial Rhythms
- Base layout increments use an uncompromising 4px spatial rhythm (`0.25rem`), allowing dense telemetry and log matrices to fit alongside multi-pane sessions.
- Inter-pane gaps are locked at `1px` via subtle structural borders (`#1e293b`), maximizing active viewport real estate.
- Component padding relies on `space-xs` (2px) and `space-sm` (4px) for compact badges and list rows, expanding only to `space-md` (8px) for window header bars and modal dialogue pads.

### Responsive & Split Behavior
- **Desktop / Multi-Monitor (Default):** Multi-pane tiling with dynamic split-ratios (horizontal, vertical, quad-matrix). The dependency graph panel docks right or bottom at a default width of 380px, collapsible with zero-latency hotkeys.
- **Tablet / Constrained Terminals (<1024px):** Graph visualization automatically collapses to an interactive single-column breadcrumb trail above the active terminal prompt. Sidebar panels become temporary overlay sheets.
- **Mobile (<640px):** Single-pane full terminal view with a condensed 28px bottom command bar for execution triggers and semantic state toggling.

## Elevation & Depth

Visual hierarchy is achieved through **tonal surface stacking** and **crisp 1px borders**. Drop shadows, glows, and blur effects are strictly prohibited to maintain performance, reduce GPU overhead, and prevent visual noise.

### Layer Hierarchy
1. **Root Layer (`#0a0f18`):** Main terminal canvas, background execution grid, and inactive pane surface.
2. **Structural Layer (`#0f172a`):** Side navigation, environment variables pane, and dependency tree views. Separated by `1px solid #1e293b`.
3. **Active/Focused Layer (`#131d31`):** Currently focused terminal pane, command palette dropdown, hover inspector flyouts. Focused panes transition their boundary to `1px solid #334155` with a subtle top boundary indicator in `accent-cyan` (`#06b6d4`).
4. **Modal/Critical Overlays (`#0f172a`):** Interactive debugger sessions, keybind palettes, and process termination alerts. Framed with a distinct `1px solid #334155` border. No drop shadows; backdrop dimming is achieved via a flat `#000000` overlay at 60% opacity.

## Shapes

The design system uses a strict **Soft (level 1)** geometry:
- Standard UI containers, terminal tabs, buttons, inline badges, and input elements employ `0.25rem` (4px) radius.
- System modals, terminal split-frames, and graph node cards use `0.375rem` (6px) radius.
- Pill badges and fully rounded shapes are not used. Badges and chips remain geometric and rectangular with clean 2px to 4px corners to maintain an industrial, developer-focused appearance.

## Components

### Window Chrome & Split Panes
- **Frame Header:** Height fixed at 36px. Solid `#0a0f18` background with an integrated 1px bottom border `#1e293b`.
- **Window Controls:** Minimalist, non-distracting flat dots (10px diameter with 8px spacing). Close (`#f43f5e`), Minimize (`#f59e0b`), Maximize (`#10b981`). Opacity sits at 70% until header hover.
- **Tabs:** Inline trapezoidal or flush rectangular tabs. Active tab sits at `#0f172a` with a 2px top border in `accent-cyan` (`#06b6d4`). Inactive tabs are borderless against `#0a0f18` with muted text (`#64748b`).

### Buttons & Quick Actions
- **Primary Execution Button:** Background `#06b6d4`, foreground `#0a0f18`, weight 600, font `Inter`, padding 4px 10px, radius 4px. Hover shifts to `#22d3ee`.
- **Ghost / Tool Button:** Transparent background, text `#94a3b8`, border 1px solid `#1e293b`. Hover: background `#1e293b`, text `#f8fafc`.
- **Destructive Action Button:** Transparent background, text `#f43f5e`, border 1px solid `#f43f5e` at 40% opacity. Hover: background `#f43f5e`, text `#ffffff`.

### Semantic Badges & Chips
- **Graph Node / Project Badge:** Background `rgba(6, 182, 212, 0.12)`, text `#22d3ee`, border `1px solid rgba(6, 182, 212, 0.25)`. Font: `JetBrains Mono` at 11px.
- **READY / Connected State:** Background `rgba(16, 185, 129, 0.12)`, text `#34d399`, border `1px solid rgba(16, 185, 129, 0.25)`. Prepended with an emerald static 6px indicator dot.
- **Relationship Type Chip:** Background `rgba(168, 85, 247, 0.12)`, text `#c084fc`, border `1px solid rgba(168, 85, 247, 0.25)`. Used for edge syntax like `DEPENDS_ON`, `IMPORTS`, `BINDS`.

### Command Input Field & Prompts
- **Interactive Terminal Prompt:** High-density horizontal row.
  - Left segment: Project node chip in cyan (`#06b6d4`), followed by directory branch in violet (`#a855f7`).
  - Command input: Plain text field, background transparent, caret solid block (`#06b6d4`), typography `JetBrains Mono` 13px.
  - No default text box borders; focus state indicates active pane boundary.

### Interactive Graph Inspector Cards
- **Graph Entity Card:** Surface `#131d31`, border `1px solid #1e293b`, padding 8px.
  - Header: Node name in `body-sm` (`#f8fafc`) with active semantic status dot.
  - Metadata Table: Key-value rows separated by 2px gaps. Keys in `text-muted` (`#64748b`), values in `JetBrains Mono` (`#94a3b8`).
  - Edges/Connections List: Displayed with explicit violet arrow markers (`→`) and target node IDs.

### Checkboxes, Radios, & Toggles
- **Checkbox:** 14px × 14px square, radius 2px, border `1px solid #334155`, background `#0a0f18`. Checked: background `#06b6d4`, border `#06b6d4`, checkmark icon in `#0a0f18`.
- **Toggle Switch:** Ultra-compact 28px × 16px track, border `1px solid #334155`, thumb 12px × 12px flat white `#f8fafc`. Active track turns `#10b981`.