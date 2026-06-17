#include "../headers/linked_list.h"

#include "../headers/wrappers.h"

#include "../headers/constants.h"


struct Node;

typedef struct Node Node;


struct Node {
	Node* next;
	Node* prev;
	gc_VoidPtr data;
};

static Node* Node_init(gc_VoidPtr val);

static gc_VoidPtr Node_del(Node* obj);


static Node* gc_LinkedList_get_node(gc_LinkedList* self, gc_SizeT index);


struct gc_LinkedList {
	Node* head;
	Node* tail;
	gc_SizeT length;
};

gc_LinkedList* gc_LinkedList_init() {
	gc_LinkedList* self = (gc_LinkedList*)gc_malloc(gc_sizeof(gc_LinkedList));
	if (gc_not self) {
		gc_printf("Could not create gc_LinkedList!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->head = NONE;
	self->tail = NONE;
	self->length = 0;
	return self;
}

gc_VoidPtr gc_LinkedList_del(gc_LinkedList* obj) {
	obj->head = Node_del(obj->head);
	gc_free(obj);
	return NONE;
}

gc_SizeT gc_LinkedList_length(gc_LinkedList* self) {
	return self->length;
}

gc_Bool gc_LinkedList_is_empty(gc_LinkedList* self) {
	return self->length == 0;
}

gc_Iterator* gc_LinkedList_iter(gc_LinkedList* self) {
	gc_Iterator* iter = gc_Iterator_init(self->length);
	Node* current = self->head;
	for (gc_SizeT i = 0; i < self->length; i++) {
		gc_Iterator_append(iter, current->data);
		current = current->next;
	}
	return iter;
}

gc_VoidPtr gc_LinkedList_first(gc_LinkedList* self) {
	return self->head->data;
}

gc_VoidPtr gc_LinkedList_last(gc_LinkedList* self) {
	return self->tail->data;
}

gc_VoidPtr gc_LinkedList_pop(gc_LinkedList* self, gc_SizeT index) {
	Node* current = gc_LinkedList_get_node(self, index);
	Node* prev = current->prev;
	Node* next = current->next;
	gc_VoidPtr item = current->data;
	gc_free(current);
	if (prev) {
		prev->next = next;
	} else {
		self->head = next;
	}
	if (next) {
		next->prev = prev;
	} else {
		self->tail = prev;
	}
	self->length--;
	return item;
}

gc_VoidPtr gc_LinkedList_get(gc_LinkedList* self, gc_SizeT index) {
	Node* current = gc_LinkedList_get_node(self, index);
	return current->data;
}

gc_Void gc_LinkedList_append(gc_LinkedList* self, gc_VoidPtr item) {
	Node* new_node = Node_init(item);
	if (gc_not self->head) {
		self->head = new_node;
		self->tail = new_node;
	} else {
		new_node->prev = self->tail;
		self->tail->next = new_node;
		self->tail = new_node;
	}
	self->length++;
}

gc_VoidPtr gc_LinkedList_set(gc_LinkedList* self, gc_SizeT index, gc_VoidPtr item) {
	Node* current = gc_LinkedList_get_node(self, index);
	gc_VoidPtr old = current->data;
	current->data = item;
	return old;
}

gc_Void gc_LinkedList_insert(gc_LinkedList* self, gc_SizeT index, gc_VoidPtr item) {
	Node* new_node = Node_init(item);
	if (self->length == 0) {
		self->head = new_node;
		self->tail = new_node;
		new_node->next = NONE;
		new_node->prev = NONE;
	} else if (index == self->length) {
		Node* old_tail = self->tail;
		old_tail->next = new_node;
		new_node->prev = old_tail;
		new_node->next = NONE;
		self->tail = new_node;
	} else {
		Node* current = gc_LinkedList_get_node(self, index);
		Node* prev = current->prev;
		new_node->next = current;
		new_node->prev = prev;
		current->prev = new_node;
		if (prev) {
            prev->next = new_node;
        } else {
            self->head = new_node;
        }
	}
	self->length++;
}


Node* Node_init(gc_VoidPtr val) {
	Node* self = (Node*)gc_malloc(gc_sizeof(Node));
	if (gc_not self) {
		gc_printf("Could not create Node for gc_LinkedList!\n");
		gc_exit(EXIT_NULLPTR);
	}
	self->next = NONE;
	self->prev = NONE;
	self->data = val;
	return self;
}

gc_VoidPtr Node_del(Node* obj) {
	if (obj) {
		Node* next = obj->next;
		Node* tmp = NONE;
		while (next) {
			tmp = next->next;
			gc_free(next);
			next = tmp;
		}
		gc_free(obj);
	}
	return NONE;
}


Node* gc_LinkedList_get_node(gc_LinkedList* self, gc_SizeT index) {
	Node* current = NONE;
	if (index < self->length / 2) {
		current = self->head;
		for (gc_SizeT i = 0; i < index; i++) {
			current = current->next;
		}
	} else {
		current = self->tail;
		for (gc_SizeT i = self->length - 1; i > index; i--) {
			current = current->prev;
		}
	}
	return current;
}
