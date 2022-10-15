
#ifndef TRACKLIST_H
#define TRACKLIST_H






typedef struct next_prev_list_node {
    void *data;
    struct next_prev_list_node *next;
    struct next_prev_list_node *prev;
} npl_node_t;


typedef struct double_head_tail_list {
    unsigned long cnt;
    npl_node_t *head;
    npl_node_t *tail;
} dht_list_t;




#define DHT_LIST_INIT(l) \
    do { \
        l = (dht_list_t *)malloc(sizeof(dht_list_t)); \
        if (l) \
            l->cnt = 0; \
            l->head = NULL; \
            l->tail = NULL; \
    } while(0)



#define DHT_LIST_DESTROY(l) \
    do { \
        free(l); \
        l = NULL; \
    } while(0)


#define NPL_NODE_INIT(n) \
    do { \
        n = (npl_node_t *)malloc(sizeof(npl_node_t)); \
        if (n) \
            n->data = NULL; \
            n->next = NULL; \
            n->prev = NULL; \
    } while(0)


#define dht_list_for_each(l, pos) for (pos = (l->head); pos != NULL; pos = pos->next)


#define dht_list_while_each_start(l, pos) \
        pos = l->head; \
        while (pos) { \

#define dht_list_while_each_end \
            pos = pos->next; \
        }

static inline void dht_list_remove(npl_node_t *next, npl_node_t *prev) {

  next->prev = prev;
  prev->next = next;

}



static inline int dht_list_add_node_tail(dht_list_t *l, npl_node_t *n) {

    if ( !l )
        return -1;

    if ( !l->head ) {
        n->next = NULL;
        n->prev = NULL;
        l->head = l->tail = n;
    } else {
        l->tail->next = n;
        n->next = NULL;
        n->prev = l->tail;
        l->tail = n;
    }

    ++(l->cnt);

    return 0;
}



static inline int dht_list_add_node_head(dht_list_t *l, npl_node_t *n) {

    if ( !l )
        return -1;

    n->next = l->head;
    n->prev = NULL;
    l->head->prev = n;
    l->head = n;

    ++(l->cnt);

    return 0;
}



static inline npl_node_t * npl_node_alloc(void *ndata) {

    npl_node_t *n;

    n = (npl_node_t *)malloc(sizeof(npl_node_t));
    if ( !n )
        return NULL;

    n->data = ndata;
    n->next = NULL;
    n->prev = NULL;

    return n;
}


static inline npl_node_t *dht_list_remove_node(dht_list_t *l, npl_node_t *n) {

    if ( l->head == n ) {
        if ( l->tail == l->head )
            l->head = l->tail = NULL;

    } else if ( l->tail == n ) {
        npl_node_t *pre_tail = l->tail->prev;

        pre_tail->next = NULL;
        l->tail->prev = NULL;

    } else {
        dht_list_remove(n->next, n->prev);
    }

    --(l->cnt);

    return n;
}


static inline int dht_list_func_full_free(dht_list_t *l, void func_free(void *)) {

    if ( !l )
        return -1;

    npl_node_t *pos = l->head;

    while ( pos ) {
        npl_node_t *next = pos->next;

        func_free(pos->data);
        free(pos);

        pos = next;
    }

    return 0;
}


#endif /* TRACKLIST_H */
