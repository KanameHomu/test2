#include <stdio.h>
#include<stdlib.h>


typedef struct _node {
	int sid;
	struct _node* next;
}Node;

typedef struct _list {
	int len;
	Node* hp;
}List;

void list_init(List* newList) {
	newList->len = 0;
	newList->hp = (Node*)malloc(sizeof(Node));
	if (!newList->hp) {
		printf("Can not create new list,sorry.");
		exit(-1);
	}
	newList->hp->next = NULL;
}

void list_insert(List* nowList, Node* nowNode) {
	nowList->len++;
	nowNode->next = nowList->hp->next;
	nowList->hp->next = nowNode;
}

Node* list_select(List* nowList, int id) {
	Node* tempNode = nowList->hp->next;
	while (tempNode) {
		if (tempNode->sid == id)break;
		tempNode = tempNode->next;
	}

	return tempNode;
}

void list_node_delect(List* nowList, int id) {
	Node* tempNode = nowList->hp->next;
	Node* preNode = nowList->hp;

	while (tempNode) {
		if (tempNode->sid == id) {
			preNode->next = tempNode->next;
			(nowList->len)--;
			free(tempNode);
			break;
		}
		tempNode = tempNode->next;
		preNode = preNode->next;
	}
}

int main() {
	List a;
	list_init(&a);
	for (int i = 1; i <= 10; i++) {
		Node* nod = (Node*)malloc(sizeof(Node));
		if (!nod)printf("aaa");
		nod->sid = i;
		list_insert(&a, nod);
	}

	Node* temp = a.hp->next;
	while (temp) {
		printf("%d",temp->sid);
		temp = temp->next;
	}


}
