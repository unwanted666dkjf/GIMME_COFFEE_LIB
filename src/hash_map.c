#include "../headers/hash_map.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct gc_HashMap {
	gc_VoidPtr* array;
	gc_SizeT size;
	gc_SizeT length;
	gc_Floating expand_k;
	gc_Floating occupation_k;
};


struct gc_HashMapItem {
	gc_VoidPtr key;
	gc_VoidPtr value;
	gc_SizeT hash;
};


struct gc_HashMapItems {
	gc_HashMapItem** items;
	gc_SizeT length;
	gc_SizeT ind_next;
	gc_Int ind_last;
};


static gc_SizeT get_index(gc_SizeT hash, gc_SizeT size);

static gc_SizeT gc_HashMap_hash_to_index(gc_HashMap* self, gc_SizeT hash);


static gc_HashMapItems* gc_HashMapItems_init(gc_HashMap* hash_map);

static gc_HashMapItem* gc_HashMapItem_init(gc_VoidPtr key, gc_VoidPtr value, gc_SizeT hash);

static gc_VoidPtr gc_HashMapItem_del(gc_HashMapItem* obj);


gc_HashMap* gc_HashMap_init(gc_SizeT size, gc_Floating expand_k, gc_Floating occupation_k) {
	gc_HashMap* self = (gc_HashMap*)gc_malloc(gc_sizeof(gc_HashMap));
	if (gc_not self) {
		gc_printf("Could not create gc_HashMap!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->size = size;
	self->array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * self->size);
	if (gc_not self->array) {
		gc_printf("Could not create array for gc_HashMap!\n");
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

gc_VoidPtr gc_HashMap_del(gc_HashMap* obj) {
	for (gc_SizeT i = 0; i < obj->size; i++) {
		if (obj->array[i]) {
			gc_HashMapItem* item = (gc_HashMapItem*)(obj->array[i]);
			obj->array[i] = gc_HashMapItem_del(item);
		}
	}
	gc_free(obj->array);
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_HashMap_size(gc_HashMap* self) {
	return self->size;
}

gc_SizeT gc_HashMap_length(gc_HashMap* self) {
	return self->length;
}

gc_Bool gc_HashMap_is_empty(gc_HashMap* self) {
	return self->length == 0;
}

gc_Bool gc_HashMap_is_dense(gc_HashMap* self) {
	gc_SizeT max_length = (gc_SizeT)(self->size * self->occupation_k);
	return self->length >= max_length;
}

gc_Bool gc_HashMap_is_sparse(gc_HashMap* self) {
	gc_SizeT dense_len = (gc_SizeT)(self->size * self->occupation_k);
	gc_SizeT min_length = (gc_SizeT)(self->size / self->expand_k);
	gc_SizeT crit_length = (dense_len > min_length) ? min_length : dense_len;
	return self->length < crit_length;
}

gc_Bool gc_HashMap_in(gc_HashMap* self, gc_SizeT hash) {
	gc_SizeT index = gc_HashMap_hash_to_index(self, hash);
	if (self->array[index]) {
		return True;
	}
	return False;
}

gc_Void gc_HashMap_shrink(gc_HashMap* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size / self->expand_k);
	if ((gc_SizeT)(new_size * self->occupation_k) > self->length) {
		gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
		if (gc_not new_array) {
			gc_printf("Could not shrink gc_HashMap!\n");
			gc_exit(EXIT_NULLPTR);
		}
		for (gc_SizeT i = 0; i < new_size; i++) {
			new_array[i] = NONE;
		}
		gc_SizeT length = 0;
		for (gc_SizeT i = 0; i < self->size; i++) {
			if (self->array[i]) {
				gc_HashMapItem* item = (gc_HashMapItem*)(self->array[i]);
				gc_SizeT index = get_index(item->hash, new_size);
				new_array[index] = item;
				length++;
			}
		}
		gc_free(self->array);
		self->array = new_array;
		self->size = new_size;
		self->length = length;
	}
}

gc_Void gc_HashMap_expand(gc_HashMap* self) {
	gc_SizeT new_size = (gc_SizeT)(self->size * self->expand_k) + 1;
	gc_VoidPtr* new_array = (gc_VoidPtr*)gc_malloc(gc_sizeof(gc_VoidPtr) * new_size);
	if (gc_not new_array) {
		gc_printf("Could not expand gc_HashMap!\n");
		gc_exit(EXIT_NULLPTR);
	}
	for (gc_SizeT i = 0; i < new_size; i++) {
		new_array[i] = NONE;
	}
	for (gc_SizeT i = 0; i < self->size; i++) {
		if (self->array[i]) {
			gc_HashMapItem* item = (gc_HashMapItem*)(self->array[i]);
			gc_SizeT index = get_index(item->hash, new_size);
			new_array[index] = item;
		}
	}
	gc_free(self->array);
	self->array = new_array;
	self->size = new_size;
}

gc_Void gc_HashMap_add(gc_HashMap* self, gc_SizeT hash, gc_VoidPtr key, gc_VoidPtr value) {
	gc_HashMapItem* item = gc_HashMapItem_init(key, value, hash);
	gc_SizeT index = get_index(item->hash, self->size);
	self->array[index] = item;
	self->length++;
}

gc_Iterator* gc_HashMap_iter(gc_HashMap* self) {
	return gc_HashMap_keys(self);
}

gc_Iterator* gc_HashMap_keys(gc_HashMap* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	gc_SizeT cntr = 0;
	for (gc_SizeT i = 0; i < self->size; i++) {
		if (self->array[i]) {
			gc_HashMapItem* item = (gc_HashMapItem*)self->array[i];
			gc_Iterator_append(iter, item->key);
			cntr++;
		}
		if (cntr >= self->length) {
			break;
		}
	}
	return iter;
}

gc_Iterator* gc_HashMap_values(gc_HashMap* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	gc_SizeT cntr = 0;
	for (gc_SizeT i = 0; i < self->size; i++) {
		if (self->array[i]) {
			gc_HashMapItem* item = (gc_HashMapItem*)self->array[i];
			gc_Iterator_append(iter, item->value);
			cntr++;
		}
		if (cntr >= self->length) {
			break;
		}
	}
	return iter;
}

gc_VoidPtr gc_HashMap_get(gc_HashMap* self, gc_SizeT hash) {
	gc_SizeT index = get_index(hash, self->size);
	gc_HashMapItem* item = (gc_HashMapItem*)(self->array[index]);
	return item->value;
}

gc_VoidPtr gc_HashMap_pop(gc_HashMap* self, gc_SizeT hash) {
	gc_SizeT index = get_index(hash, self->size);
	gc_HashMapItem* item = (gc_HashMapItem*)(self->array[index]);
	gc_VoidPtr value = item->value;
	self->array[index] = gc_HashMapItem_del(item);
	self->length--;
	return value;
}

gc_HashMapItems* gc_HashMap_items(gc_HashMap* self) {
	return gc_HashMapItems_init(self);
}


gc_HashMapItems* gc_HashMapItems_init(gc_HashMap* hash_map) {
	gc_HashMapItems* self = (gc_HashMapItems*)gc_malloc(gc_sizeof(gc_HashMapItems));
	if (gc_not self) {
		gc_printf("Could not create gc_HashMapItems!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->length = hash_map->length;
	self->items = (gc_HashMapItem**)gc_malloc(gc_sizeof(gc_HashMapItem*) * self->length);
	if (gc_not self->items) {
		gc_printf("Could not create array for gc_HashMapItems!\n");
		gc_exit(EXIT_NULLPTR);
	}
	gc_SizeT j = 0;
	gc_SizeT cntr = 0;
	for (gc_SizeT i = 0; i < hash_map->size; i++) {
		if (hash_map->array[i]) {
			self->items[j] = (gc_HashMapItem*)(hash_map->array[i]);
			j++;
			cntr++;
		}
		if (cntr >= self->length) {
			break;
		}
	}
	self->ind_next = 0;
	self->ind_last = self->length - 1;
	return self;
}

gc_VoidPtr gc_HashMapItems_del(gc_HashMapItems* obj) {
	gc_free(obj->items);
	gc_free(obj);
	return NONE;
}

gc_Bool gc_HashMapItems_has_next(gc_HashMapItems* self) {
	return self->ind_next < self->length;
}

gc_Bool gc_HashMapItems_has_last(gc_HashMapItems* self) {
	return self->ind_last >= 0;
}

gc_HashMapItem* gc_HashMapItems_next(gc_HashMapItems* self) {
	gc_HashMapItem* item = self->items[self->ind_next];
	self->items[self->ind_next] = NONE;
	self->ind_next++;
	return item;
}

gc_HashMapItem* gc_HashMapItems_last(gc_HashMapItems* self) {
	gc_HashMapItem* item = self->items[self->ind_last];
	self->items[self->ind_last] = NONE;
	self->ind_last--;
	return item;
}


gc_HashMapItem* gc_HashMapItem_init(gc_VoidPtr key, gc_VoidPtr value, gc_SizeT hash) {
	gc_HashMapItem* self = (gc_HashMapItem*)gc_malloc(gc_sizeof(gc_HashMapItem));
	if (gc_not self) {
		gc_printf("Could not create gc_HashMapItem!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->key = key;
	self->value = value;
	self->hash = hash;
	return self;
}

gc_VoidPtr gc_HashMapItem_del(gc_HashMapItem* obj) {
	gc_free(obj);
	return NONE;
}

gc_VoidPtr gc_HashMapItem_key(gc_HashMapItem* self) {
	return self->key;
}

gc_VoidPtr gc_HashMapItem_value(gc_HashMapItem* self) {
	return self->value;
}

gc_SizeT gc_HashMapItem_hash(gc_HashMapItem* self) {
	return self->hash;
}


gc_SizeT get_index(gc_SizeT hash, gc_SizeT size) {
	return hash % size;
}

gc_SizeT gc_HashMap_hash_to_index(gc_HashMap* self, gc_SizeT hash) {
	return get_index(hash, self->size);
}
