/**
 * \file   libc_backends/l4re_mem/mem.cc
 */
/*
 * (c) 2004-2009 Technische Universität Dresden
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU Lesser General Public License 2.1.
 * Please see the COPYING-LGPL-2.1 file for details.
 */

#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <pthread.h>

#ifdef TEST_ENV
#include <cstdio>
#else
#include <cstdio>
#include <l4/sys/kdebug.h>
#include <l4/re/rm>
#include <l4/re/env>
#include <l4/re/mem_alloc>
#include <l4/re/dataspace>
#include <l4/re/util/cap_alloc>

#define my_malloc malloc
#define my_free free
#define my_realloc realloc
#endif


const uint32_t CHUNK_BITS = 21;   // 2 MiB
const uint32_t CHUNK_SIZE = (1 << CHUNK_BITS);
const uint32_t CHUNK_MASK = CHUNK_SIZE - 1;

const uint32_t ALLOC_BITS = 3;    // 8 Bytes
const uint32_t MIN_ALLOC_SIZE = (1 << ALLOC_BITS);
const uint32_t MAX_ALLOC_SIZE = (CHUNK_SIZE >> 1);

const uint32_t PATH_BITS = (CHUNK_BITS - ALLOC_BITS);

struct Slab;
typedef Slab *Path[CHUNK_BITS];

struct Slab {
    // bits [0 .. 17] save the node index, in case this slab was split
    static const uint32_t INDEX_MASK = (1 << PATH_BITS) - 1;
    // bits [18 .. 22] save the level of the largest free sub-slab
    //                  if no free sub-slab exists this bits [18..22] are set to 1
    static const uint32_t SPLIT_MASK = (1 << 5) - 1;
    // bits [23 .. 29] are unused
    // bit [30] signifies if this slab was split before (i.e. whether index refers to an unused node)
    static const uint32_t WAS_SPLIT_BIT = (1 << 30);
    // bit [31] signifies if this slab is currently split (i.e. whether index is valid)
    static const uint32_t IS_SPLIT_BIT = (1 << 31);

    inline uint32_t index() const { return inner & INDEX_MASK; }
    inline void index(uint32_t index) {
        inner &= ~INDEX_MASK;
        inner |= (index & INDEX_MASK);
    }

    inline uint32_t splits() const { return (inner >> PATH_BITS) & SPLIT_MASK; }
    inline void splits(uint32_t splits) {
        inner &= ~(SPLIT_MASK << PATH_BITS);
        inner |= (splits & SPLIT_MASK) << PATH_BITS;
    }

    inline bool is_split() const { return inner & IS_SPLIT_BIT; }
    inline void is_split(bool is_split) {
        if (is_split) inner |= IS_SPLIT_BIT;
        else inner &= ~IS_SPLIT_BIT;
    }

    inline bool was_split() const { return inner & WAS_SPLIT_BIT; }
    inline void was_split(bool was_split) {
        if (was_split) inner |= WAS_SPLIT_BIT;
        else inner &= ~WAS_SPLIT_BIT;
    }

    uint32_t inner;
};

struct Node {
    Slab left, right;
};

struct MetaChunk {
    static const uint32_t CHUNKS = 8;
    static const uint32_t MAX_NODES;

    MetaChunk *next;
    uint32_t num_nodes;
    uint32_t num_roots;
    Slab roots[CHUNKS];
    Node nodes[];

    void *allocate(size_t alloc_size);
    uint32_t find_node(void *p, Path path);
};

const uint32_t MetaChunk::MAX_NODES =
    (CHUNK_SIZE - offsetof(MetaChunk, nodes)) / sizeof(Node);

static_assert(MetaChunk::MAX_NODES <= Slab::INDEX_MASK);

struct Allocator {
    MetaChunk *meta;
    pthread_mutex_t mutex;

    uint32_t find_pointer(void *p, MetaChunk **meta, Path path);
};

static Allocator allocator;

static MetaChunk *allocate_meta() {
#ifdef TEST_ENV
    MetaChunk *meta = (MetaChunk*) malloc(CHUNK_SIZE * 2);
#else
    auto ds = L4Re::Util::cap_alloc.alloc<L4Re::Dataspace>();

    if (!ds.is_valid())
        return nullptr;

    // allocate one MetaChunk + one regular chunk
    int result = L4Re::Env::env()->mem_alloc()->alloc(CHUNK_SIZE * 2, ds, 0);
    if (result != 0)
        return 0;

    MetaChunk *meta = nullptr;
    // result = L4Re::Env::env()->rm()->reserve_area(
    //     &meta,
    //     // as many chunks as managed by a single MetaChunk
    //     CHUNK_SIZE * (MetaChunk::CHUNKS + 1),
    //     L4Re::Rm::F::Search_addr | L4Re::Rm::F::RW,
    //     CHUNK_BITS
    // );

    result = L4Re::Env::env()->rm()->attach(
        &meta,
        CHUNK_SIZE*2,
        L4Re::Rm::F::Search_addr | L4Re::Rm::F::RW,
        ds
    );
#endif

    memset(meta, 0, sizeof(MetaChunk));
    meta->num_roots = 1;

    return meta;
}

inline size_t align(size_t size) {
    if (size == 0)
        return 0;

    return (((size - 1) >> ALLOC_BITS) + 1) << ALLOC_BITS;
}

void *MetaChunk::allocate(size_t size) {
    if (size == 0) return nullptr;
    size = align(size);

    unsigned chunk_num = 0;
    size_t chunk_offset = 0;

    Slab *root = nullptr;

    while (chunk_num < num_roots) {
        root = &roots[chunk_num];

        if ((CHUNK_SIZE >> root->splits()) < size) {
            chunk_num++;
            continue;
        }

        break;
    }

    if (chunk_num == CHUNKS) {
        fprintf(stderr, "oom\n");
        // TODO: allocate new meta chunk
        return nullptr;
    }

    if (chunk_num == num_roots) {
        fprintf(stderr, "oom\n");
        // TODO: allocate new chunk
        return nullptr;
    }

    Path path = { nullptr };

    unsigned depth = 0;
    Slab *current = root;

    // find unused slab close to the needed size
    while ((path[depth] = current)->is_split()) {
        depth++;

        // retrieve the immediate sub-slabs for this slab
        Node *node = &nodes[current->index()];

        // choose left or right (which one is closer to size?)
        size_t left_size = CHUNK_SIZE >> (node->left.splits() + depth);
        size_t right_size = CHUNK_SIZE >> (node->right.splits() + depth);

        if (right_size < size || (left_size >= size && left_size <= right_size)) {
            // choose the left node
            current = &node->left;
        }
        else {
            // choose the right node
            current = &node->right;
            chunk_offset |= (CHUNK_SIZE >> depth);
        }
    }

    assert((CHUNK_SIZE >> depth) >= size);

    // split the current slab to match
    while ((CHUNK_SIZE >> (depth + 1)) >= size) {
        // reuse the node from the last time this slab was split
        uint32_t node_index = current->index();

        if (!current->was_split()) {
            // if this slab was never split before, allocate a new node
            if (num_nodes == MAX_NODES) {
                // out of nodes, overallocate as a fallback
                break;
            }

            node_index = num_nodes++;
            nodes[node_index] = Node { 0, 0 };
            current->index(node_index);
        }

        current->is_split(true);
        current->splits(1);

        current = &nodes[node_index].left;
        depth++;
        path[depth] = current;
    }

    current->splits(Slab::SPLIT_MASK);

    // walk back up the path updating splits
    while (depth > 0) {
        depth--;
        current = path[depth];

        uint32_t left_splits = nodes[current->index()].left.splits();
        uint32_t right_splits = nodes[current->index()].right.splits();

        if (left_splits == Slab::SPLIT_MASK && right_splits == Slab::SPLIT_MASK) {
            // if both children are fully used, this slab is fully used
            current->splits(Slab::SPLIT_MASK);
        } else if (left_splits <= right_splits) {
            // larger slab on the left hand side
            current->splits(left_splits + 1);
        } else {
            // larger slab on the right hand side
            current->splits(right_splits + 1);
        }
    }

    return (void*)((size_t)this + ((chunk_num + 1) * CHUNK_SIZE) + chunk_offset);
}

uint32_t MetaChunk::find_node(void *p, Path path)
{
    memset(path, 0, sizeof(Slab*) * CHUNK_BITS);

    if (this > p) return 0;

    size_t offset = (size_t)p - (size_t)this;
    size_t root_index = (offset / CHUNK_SIZE) - 1;

    if (root_index >= CHUNKS) return 0;

    path[0] = &roots[root_index];

    uint32_t depth = 0;
    while (path[depth]->is_split()) {
        uint32_t node_idx = path[depth++]->index();
        size_t mask = 1 << (CHUNK_BITS - depth);

        if (offset & mask) {
            path[depth] = &nodes[node_idx].right;
        }
        else {
            path[depth] = &nodes[node_idx].left;
        }
    }

    return depth;
}

uint32_t Allocator::find_pointer(void *p, MetaChunk **meta, Path path) {
    uint32_t depth = 0;
    *meta = this->meta;
    path = (Path){ 0 };

    for (; *meta && path[0] == nullptr; (*meta = (*meta)->next)) {
        depth = (*meta)->find_node(p, path);
    }

    return depth;
}

void *my_malloc(size_t size) noexcept
{
    pthread_mutex_lock(&allocator.mutex);

    if (allocator.meta == nullptr) {
        allocator.meta = allocate_meta();
    }

    void *result = allocator.meta->allocate(size);
    pthread_mutex_unlock(&allocator.mutex);
    printf("allocated %d bytes @ 0x%x\n", size, result);
    return result;
}

void my_free(void *p) noexcept
{
    pthread_mutex_lock(&allocator.mutex);
    Path path;
    MetaChunk *meta;
    uint32_t depth = allocator.find_pointer(p, &meta, path);

    if (!meta) {
        pthread_mutex_unlock(&allocator.mutex);
        return;
    }

    Slab *current = path[depth];
    current->splits(0);

    // unsplit parent nodes
    while (depth > 0) {
        depth--;
        current = path[depth];
        Node node = meta->nodes[current->index()];

        if (node.left.splits() == 0 && node.right.splits() == 0) {
            current->was_split(true);
            current->is_split(false);
            current->splits(0);
        }
        else if (node.left.splits() < current->splits() - 1) {
            current->splits(node.left.splits() + 1);
        }
        else if (node.right.splits() < current->splits() - 1) {
            current->splits(node.right.splits() + 1);
        }
        // unchanged, so no further changes for parents
        else break;
    }

    pthread_mutex_unlock(&allocator.mutex);
}

void *my_realloc(void *p, size_t size) noexcept
{
    pthread_mutex_lock(&allocator.mutex);
    Path path = { nullptr };
    MetaChunk *meta;
    uint32_t depth = allocator.find_pointer(p, &meta, path);
    pthread_mutex_unlock(&allocator.mutex);

    if (size <= (CHUNK_SIZE >> depth)) {
        // already large enough
        return p;
    }

    void *res = my_malloc(size);
    if (!res) return res;

    memcpy(res, p, size);
    my_free(p);
    return res;
}

#ifdef TEST_ENV
void print_slab(Slab slab) {
    printf("\t{ is_split = %d, was_split = %d, splits = %d, index = %d }\n",
            slab.is_split(), slab.was_split(), slab.splits(), slab.index());
}

int main(int argc, const char *argv[])
{
    void *large = my_malloc(64 * 1024);
    void *small_nodes[5] = { nullptr };

    for (unsigned i = 0; i < 5; i++)
        small_nodes[i] = my_malloc(4096);

    print_slab(allocator.meta->roots[0]);

    for (unsigned i = 0; i < allocator.meta->num_nodes; i++) {
        Node node = allocator.meta->nodes[i];
        printf("%d.left: ", i);
        print_slab(node.left);
        printf("%d.right: ", i);
        print_slab(node.right);
    }

    Path path = { nullptr };
    uint32_t depth = allocator.meta->find_node(small_nodes[4], path);

    for (Slab **curr = path; *curr; curr++)
        print_slab(*curr[0]);

    printf("size = %d\n", CHUNK_SIZE >> depth);

    my_free(small_nodes[4]);

    void *thing = my_malloc(16 * 1024);
    my_free(thing);

    print_slab(allocator.meta->roots[0]);

    for (unsigned i = 0; i < allocator.meta->num_nodes; i++) {
        Node node = allocator.meta->nodes[i];
        printf("%d.left: ", i);
        print_slab(node.left);
        printf("%d.right: ", i);
        print_slab(node.right);
    }

	return 0;
}
#endif


