#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _node {
	int data;
	struct _node *next;
} Node;

typedef struct _queue {
	int len;
	Node *hp;
	Node *tp;
} Queue;

void queue_init(Queue *a) {
	a->hp = 0;
	a->hp = a->tp = (Node *)malloc(sizeof(Node));
	if (!a->hp || !a->tp) {
		printf("Can not create Queue!");
		exit(-1);
	}

	a->hp->next = NULL;
	a->len = 0;

}

bool queue_is_empty(Queue *a) {
	if (a->hp == a->tp)
		return true;
	else
		return false;
}

void queue_insert(Queue *a, int data) {
	Node *temp = (Node *)malloc(sizeof(Node));
	if (!temp) {
		printf("Can not insert!");
		exit(-1);
	}
	temp->data = data;

	temp->next = NULL;
	a->tp->next = temp;
	a->tp = temp;
	a->len++;
}

void queue_print(Queue *a) {
	Node *temp = a->hp->next;

	for (int i = 0; i < a->len && temp; i++) {
		printf("%d ", temp->data);
		temp = temp->next;
	}
}

bool queue_out(Queue *a, int *b) {
	if (queue_is_empty(a)) {
		printf("NO!");
		return false;

	}

	else {
		Node *temp = a->hp->next;
		*b = temp->data;
		a->hp->next = temp->next;
		if (a->tp == temp) {
			a->hp = a->tp;
		}
		free(temp);
		return true;
	}
}

int main() {
	Queue team;
	queue_init(&team);






	return 0;
}