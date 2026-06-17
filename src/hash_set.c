#include "../headers/hash_set.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


const gc_SizeT SAFE_HASH_SET_SIZE = 32;

const gc_Floating SAFE_HASH_SET_EXPAND_K = 8;

const gc_Floating SAFE_HASH_SET_OCCUPATION_K = .4;


struct HashSetItem;

typedef struct HashSetItem HashSetItem;


struct HashSetItem {
	gc_VoidPtr item;
	gc_SizeT hash;
};

static HashSetItem* HashSetItem_init(gc_VoidPtr item, gc_SizeT hash);

static gc_VoidPtr HashSetItem_del(HashSetItem* obj);


static gc_SizeT get_index(gc_SizeT hash, gc_SizeT size);

static gc_SizeT gc_HashSet_hash_to_index(gc_HashSet* self, gc_SizeT hash);


struct gc_HashSet {
	gc_VoidPtr* array;
	gc_SizeT size;
	gc_SizeT length;
	gc_Floating expand_k;
	gc_Floating occupation_k;
};

gc_HashSet* gc_HashSet_init(gc_SizeT size, gc_Floating expand_k, gc_Floating occupation_k) {
	gc_HashSet* self = (gc_HashSet*)gc_malloc(gc_sizeof(gc_HashSet));
	if (gc_not self) {
		gc_printf("Could not create gc_HashSet!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size;
	self->array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_HashSet!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < self->size; i++) {
		self->array[i] = NONE;
	}
	self->length = 0;
	self->expand_k = expand_k;
	self->occupation_k = occupation_k;
	return self;
}

gc_VoidPtr gc_HashSet_del(gc_HashSet* obj) {
	for (gc_SizeT i = 0; i < obj->size; i++) {
		if (obj->array[i]) {
			HashSetItem* item = (HashSetItem*)obj->array[i];
			obj->array[i] = HashSetItem_del(item);
		}
	}
	gc_free(obj->array);
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_HashSet_size(gc_HashSet* self) {
	return self->size;
}

gc_SizeT gc_HashSet_length(gc_HashSet* self) {
	return self->length;
}

gc_Void gc_HashSet_expand(gc_HashSet* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size * self->expand_k) + 1;
	gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
	if (gc_not new_array) {
		gc_printf("Could not expand gc_HashSet!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < new_size; i++) {
		new_array[i] = NONE;
	}
	for (gc_SizeT i = 0; i < self->size; i++) {
		if (self->array[i]) {
			HashSetItem* item = (HashSetItem*)self->array[i];
			gc_SizeT index = get_index(item->hash, new_size);
			new_array[index] = item;
		}
	}
	gc_free(self->array);
	self->size = new_size;
	self->array = new_array;
}

gc_Void gc_HashSet_shrink(gc_HashSet* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size / self->expand_k);
	if ((gc_SizeT)(new_size * self->occupation_k) > self->length) {
		gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
		if (gc_not new_array) {
			gc_printf("Could not shrink gc_HashSet!\n");
			gc_exit(EXIT_NULLPTR);
		}
		for (gc_SizeT i = 0; i < new_size; i++) {
			new_array[i] = NONE;
		}
		gc_SizeT length = 0;
		for (gc_SizeT i = 0; i < self->size; i++) {
			if (self->array[i]) {
				HashSetItem* item = (HashSetItem*)self->array[i];
				gc_SizeT index = get_index(item->hash, new_size);
				new_array[index] = item;
				length++;
			}
		}
		gc_free(self->array);
		self->size = new_size;
		self->length = length;
		self->array = new_array;
	}
}

gc_Iterator* gc_HashSet_iter(gc_HashSet* self) {
	gc_SizeT cntr = 0;
	gc_Iterator* iter = gc_Iterator_init(self->length);
	for (gc_SizeT i = 0; i < self->size; i++) {
		if (self->array[i]) {
			HashSetItem* item = (HashSetItem*)self->array[i];
			gc_Iterator_append(iter, item->item);
			cntr++;
		}
		if (cntr >= self->length) {
			break;
		}
	}
	return iter;
}

gc_Void gc_HashSet_add(gc_HashSet* self, gc_SizeT hash, gc_VoidPtr item) {
	gc_SizeT index = gc_HashSet_hash_to_index(self, hash);
	self->array[index] = HashSetItem_init(item, hash);
	self->length++;
}

gc_Bool gc_HashSet_is_empty(gc_HashSet* self) {
	return self->length == 0;
}

gc_Bool gc_HashSet_is_dense(gc_HashSet* self) {
	gc_SizeT max_length = (gc_SizeT)((gc_Floating)(self->size) * self->occupation_k);
	return self->length >= max_length;
}

gc_Bool gc_HashSet_is_sparse(gc_HashSet* self) {
	gc_SizeT dense_len = (gc_SizeT)(self->size * self->occupation_k);
	gc_SizeT min_length = (gc_SizeT)(self->size / self->expand_k);
	gc_SizeT crit_length = (dense_len > min_length) ? min_length : dense_len;
	return self->length < crit_length;
}

gc_Bool gc_HashSet_in(gc_HashSet* self, gc_SizeT hash) {
	gc_SizeT index = gc_HashSet_hash_to_index(self, hash);
	if (self->array[index]) {
		return True;
	}
	return False;
}

gc_VoidPtr gc_HashSet_get(gc_HashSet* self, gc_SizeT hash) {
	gc_SizeT index = gc_HashSet_hash_to_index(self, hash);
	HashSetItem* item = (HashSetItem*)self->array[index];
	return item->item;
}

gc_VoidPtr gc_HashSet_pop(gc_HashSet* self, gc_SizeT hash) {
	gc_SizeT index = gc_HashSet_hash_to_index(self, hash);
	HashSetItem* item = (HashSetItem*)self->array[index];
	gc_VoidPtr old = item->item;
	self->array[index] = HashSetItem_del(item);
	self->length--;
	return old;
}


HashSetItem* HashSetItem_init(gc_VoidPtr item, gc_SizeT hash) {
	HashSetItem* self = (HashSetItem*)gc_malloc(gc_sizeof(HashSetItem));
	if (gc_not self) {
		gc_printf("Could not create HashSetItem for gc_HashSet!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->item = item;
	self->hash = hash;
	return self;
}

gc_VoidPtr HashSetItem_del(HashSetItem* obj) {
	gc_free(obj);
	return NONE;
}


gc_SizeT gc_HashSet_hash_to_index(gc_HashSet* self, gc_SizeT hash) {
	return get_index(hash, self->size);
}

gc_SizeT get_index(gc_SizeT hash, gc_SizeT size) {
	return hash % size;
}
