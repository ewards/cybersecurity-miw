#include <string.h>

#include "mr-types.hh"
#include "bench.hh"

int defsplitter(void *arg, split_t * ma, int ncores) {
    defsplitter_state *ds = (defsplitter_state *)arg;
    pthread_mutex_lock(&ds->mu);

    size_t split_pos = ds->split_pos;
    char *mr_data = (char *) ds->data;
    size_t data_size = ds->data_size;
    if (split_pos >= data_size) {
	pthread_mutex_unlock(&ds->mu);
	return 0;
    }
    if (ds->nsplits == 0)
	ds->nsplits = ncores * def_nsplits_per_core;
    uint64_t split_size = ds->data_size / ds->nsplits;
    split_size = round_down(split_size, ds->align);
    ma->data = (void *) &mr_data[split_pos];
    if (split_pos + split_size > data_size)
	ma->length = data_size - split_pos;
    else
	ma->length = split_size;
    ds->split_pos += split_size;
    pthread_mutex_unlock(&ds->mu);
    return 1;
}

void defsplitter_init(struct defsplitter_state *ds, void *data,
		      size_t data_size, uint64_t nsplits, size_t align) {
    bzero(ds, sizeof(*ds));
    assert(data_size % align == 0);
    ds->data_size = data_size;
    ds->data = data;
    ds->nsplits = nsplits;
    ds->align = align;
    pthread_mutex_init(&ds->mu, 0);
}
