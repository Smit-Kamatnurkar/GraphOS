# GraphOS — Top-level Makefile
# ────────────────────────────

CC       = gcc
CFLAGS   = -Wall -Wextra -Wpedantic -std=c99
CFLAGS  += -I graph-core/include

# Build directory
BUILD    = build

# ── Sources ──────────────────────────────────────────
CORE_SRC = graph-core/src/graph.c
TEST_SRC = graph-core/src/test.c

# ── Objects ──────────────────────────────────────────
CORE_OBJ = $(BUILD)/graph.o
TEST_OBJ = $(BUILD)/test.o

# ── Targets ──────────────────────────────────────────
LIB      = $(BUILD)/libgraph.a
TEST_BIN = $(BUILD)/graph_test
GRAPHFS_BIN = $(BUILD)/graphfs
GRAPHCTL_BIN = $(BUILD)/graphctl

# ═══════════════════════════════════════════════════════
# Default: build everything
# ═══════════════════════════════════════════════════════
.PHONY: all
all: $(TEST_BIN) $(GRAPHFS_BIN) $(GRAPHCTL_BIN)

# ── Static library ───────────────────────────────────
$(CORE_OBJ): $(CORE_SRC) graph-core/include/graph.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(CORE_OBJ)
	ar rcs $@ $<

# ── Test binary ──────────────────────────────────────
$(TEST_OBJ): $(TEST_SRC) graph-core/include/graph.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

# ── GraphFS (FUSE 3) ────────────────────────────────
GRAPHFS_SRC = graphfs/src/graphfs.c
GRAPHFS_OBJ = $(BUILD)/graphfs.o
FUSE_CFLAGS = $(shell pkg-config --cflags fuse3)
FUSE_LIBS   = $(shell pkg-config --libs fuse3)

$(GRAPHFS_OBJ): $(GRAPHFS_SRC) graph-core/include/graph.h | $(BUILD)
	$(CC) $(CFLAGS) $(FUSE_CFLAGS) -c $< -o $@

$(GRAPHFS_BIN): $(GRAPHFS_OBJ) $(LIB)
	$(CC) $(CFLAGS) $^ $(FUSE_LIBS) -o $@

# ── graphctl (CLI Tool) ──────────────────────────────
GRAPHCTL_SRC = graphctl/src/graphctl.c
GRAPHCTL_BIN = $(BUILD)/graphctl

$(GRAPHCTL_BIN): $(GRAPHCTL_SRC) | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

# ── Build directory ──────────────────────────────────
$(BUILD):
	mkdir -p $(BUILD)

# ═══════════════════════════════════════════════════════
# Run the test
# ═══════════════════════════════════════════════════════
.PHONY: test
test: $(TEST_BIN)
	@echo "────────────────────────────"
	@./$(TEST_BIN)
	@echo "────────────────────────────"

# ═══════════════════════════════════════════════════════
# Clean build artifacts
# ═══════════════════════════════════════════════════════
.PHONY: clean
clean:
	rm -f $(BUILD)/*.o $(BUILD)/*.a $(BUILD)/graph_test $(BUILD)/graphfs $(BUILD)/graphctl
