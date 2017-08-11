#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define FILE_NAME "data" // file name used as both input and output
#define CLS system("cls")

typedef struct GraphNode {
	int id;
	float x;
	float y;
	struct GraphNode *next;
	struct LNode *edges;
} GNode;

typedef struct ListNode {
	int id;
	struct ListNode *next;
} LNode;

// global variable - holding a reference to head of the graph
GNode* HEAD = NULL;

// create a new EdgLNode node which only has id of the dest graph node
LNode* createListNode(int id_) {
	LNode* newLNode = (LNode *)malloc(sizeof(LNode));
	if (newLNode == NULL) {
		printf("memory error! :(");
		exit(EXIT_FAILURE);
	}
	else {
		newLNode->id = id_;
		newLNode->next = NULL;
	}
	return newLNode;
}

// adds a EdgeNode to a given list
void addToEndOfEdgeList(GNode* node, LNode* newEdgeNode) {
	if (node->edges == NULL) {
		node->edges = newEdgeNode;
		return;
	}
	// pointing to the current node in the edge list
	LNode* cursor = node->edges;

	// moving to the end of list
	while (cursor->next != NULL) cursor = cursor->next;

	// insert the node here
	cursor->next = newEdgeNode;
}

// creates a new GraphNode and sets its values
// after creation no other node is linked to this one
GNode* createNode(int id_, float x_, float y_) {
	GNode* newNode = (GNode *)malloc(sizeof(GNode));
	if (newNode == NULL) {
		printf("Memory Error! :(\n");
		return 0;
	}
	else {
		newNode->id = id_;
		newNode->x = x_;
		newNode->y = y_;
		newNode->next = NULL;
		newNode->edges = NULL;
	}
	return newNode;
}
// add node to end of a graph
void addToEndOfGraphList(GNode* head, GNode* node) {
	// cursor to point to current position at node list
	GNode *cursor = head;
	// move cursor to the end of list
	while (cursor->next != NULL) cursor = cursor->next;

	cursor->next = node;
}

// prints information of a given node of graph
void printNode(GNode *node) {
	printf("%d : (%f, %f) -> ", node->id, node->x, node->y);
	if (node->edges != NULL) {
		LNode* nextEdge = node->edges;
		printf("%d", nextEdge->id);
		while (nextEdge->next != NULL) {
			nextEdge = nextEdge->next;
			printf(", %d", nextEdge->id);
		}
		printf("\n");
	}
	else {
		printf("NULL\n");
	}
}

// returns a node of graph given its id
GNode* getGraphNode(int id_) {
	GNode* cursor = HEAD;
	
	// is it the head node
	if (cursor->id == id_) return cursor;
	
	// check id of each node in the graph
	while (cursor->next != NULL) {
		cursor = cursor->next;
		if (cursor->id == id_) return cursor;
	}
	// there is no node with the given id
	return NULL;
}

// remove a node(given its id) from edge list of another node
void removeFromEdgeList(GNode* node, int id_) {
	LNode* cursor = node->edges;
	if (cursor == NULL) return;
	// if it is the head of the list
	if (cursor->id == id_) {
		node->edges = cursor->next;
		free(cursor);
		return;
	}
	// if it is not head of the list
	LNode* prev = NULL;
	do {	// move cursor to the node with given id and save the previous node
		prev = cursor;
		cursor = cursor->next;
		if (cursor == NULL) return; // no edge between the given nodes
	} while (cursor->id != id_);

	prev->next = cursor->next;
	free(cursor);
}

// removes a node from the graph
// graph is represented by a global varaible HEAD
// input parameter is id_ of the node
int removeNode(int id_) {
	if (HEAD == NULL) return 0;

	if (HEAD->id == id_) {
		// save a link to next element
		GNode* tmp = HEAD->next;
		// remove head node
		if (HEAD->edges != NULL) {
			LNode* cursor = HEAD->edges;
			do {
				GNode *node = getGraphNode(cursor->id);
				removeFromEdgeList(node, cursor->id);
			} while (cursor != NULL);
		}
		HEAD = tmp;
		return 1;
	}
	
	// moving cursor to the specific node while saving a link to its previous node
	GNode* prev;
	GNode* cursor = HEAD;
	do {
		prev = cursor;
		cursor = cursor->next;
		if (cursor == NULL) return 0;	// there is no vertex with the given id
	} while (cursor->id != id_);

	prev->next = cursor->next;	// fix the links in the list
	
	// delete the node pointed by cursor
	if (cursor->edges != NULL) { // if it has edges delete them
		LNode* head = cursor->edges;
		do {
			LNode* tmp = head;
			GNode* destNode = getGraphNode(tmp->id);
			if (destNode == NULL) {
				printf("no such a node!!!");
				exit(EXIT_FAILURE);
			}
			removeFromEdgeList(destNode, cursor->id);
			head = head->next;
			free(tmp);
		} while (head != NULL);
	}
	free(cursor);
	return 1;
}

// remove an edge from the graph
int removeEdge(int id1, int id2) {
	GNode* node1 = getGraphNode(id1);	// find node of id1
	if (node1 == NULL) return 1;
	GNode* node2 = getGraphNode(id2);	// find node of id2
	if (node2 == NULL) return 2;
	removeFromEdgeList(node1, id2);		// remove node2 from node1's edge list
	removeFromEdgeList(node2, id1);		// remove node1 from node2's edge list
	return 0;
}

// a string containing comma separated vales for ID,X,Y,edgelist
GNode* parseLine(char* line) {
	const char* dlm1 = ",";
	char* token;

	// temp variables
	int tmpId;
	float tmpX, tmpY;

	// get first token which is ID
	token = strtok(line, dlm1);
	tmpId = strtol(token, 0, 0);
	// get second token which is X
	token = strtok(NULL, dlm1);
	tmpX = strtof(token, 0);
	//get third token which is Y
	token = strtok(NULL, dlm1);
	tmpY = strtof(token, 0);

	// create a new node 
	GNode* newNode = createNode(tmpId, tmpX, tmpY);
	newNode->next = NULL;

	// all other tokens would be id of end node of edges of this node
	token = strtok(NULL, dlm1);
	if (token == NULL) {
		return newNode; // no edges
	}
	tmpId = strtol(token, 0, 0);

	if (!tmpId) // if id is zero (a failed conversion would return zero too)
		return newNode;	 // the node is isolated - has no edge

	// create an edge list using first node id
	LNode* edgeList = createListNode(tmpId);

	// adding a link from graph node to the created edge list
	newNode->edges = edgeList;

	// read next token to the end of line
	for (token = strtok(NULL, dlm1); token && *token; token = strtok(NULL, dlm1))
	{
		tmpId = strtol(token, 0, 0);
		LNode* foo = createListNode(tmpId);
		addToEndOfEdgeList(newNode, foo);
	}

	return newNode;
}

// prints the whole graph node by node
void printGraph(GNode* head) {
	printf("\n");
	GNode* cursor = head;
	do {
		printNode(cursor);
		cursor = cursor->next;
	} while (cursor != NULL);
}

// return values:
// 0 -> success
// 1 -> first node is not in the graph
// 2 -> second node is not in the graph
int addEdge(int id1, int id2) {
	// find the nodes
	GNode* node1 = getGraphNode(id1);
	if (node1 == NULL) return 1;
	GNode* node2 = getGraphNode(id2);
	if (node2 == NULL) return 2;
	addToEndOfEdgeList(node1, createListNode(id2));
	addToEndOfEdgeList(node2, createListNode(id1));
	return 0;
}

// displays the menus on the screen
void dispMenu() {
	printf("\n");
	int i;
	for (i = 0; i < 10; i++) printf("-");
	printf("GRAPH OPERATIONS");
	for (i = 0; i < 10; i++) printf("-");
	printf("\n");
	printf("Choose one of the following actions:\n");
	printf("1 - Add Vertex.\n");
	printf("2 - Remove Vertex.\n");
	printf("3 - Add Edge.\n");
	printf("4 - Remove Edge.\n");
	printf("5 - Save and Exit.\n");
	printf("6 - Print Graph.\n");
}

GNode* readGraphFromFile(const char* fileName) {
	FILE* inFile = fopen("data", "r");
	char line[BUFFER_SIZE];

	// a variable that holds link to head of graph
	// which will be treated as the graph - all operations on 
	// the graph will be done using this variable
	GNode* gHead = NULL;

	while (fgets(line, BUFFER_SIZE, inFile)) {
		GNode* curNode = parseLine(line);
		if (gHead == NULL) {
			gHead = curNode;
		}
		else {
			addToEndOfGraphList(gHead, curNode);
		}
	}

	fclose(inFile);
	return gHead;
}

void saveGraph(const char* fileName) {
	
	FILE* fp = fopen(fileName, "w");
	if (HEAD == NULL) {
		fclose(fp);
		return;
	}
	GNode* cursor = HEAD;	// will move node by node in the graph
	do {
		fprintf(fp, "%d,%f,%f", cursor->id, cursor->x, cursor->y);
		if (cursor->edges == NULL)
			fprintf(fp, ",");
		else {
			LNode* lCursor = cursor->edges; // will move on the edge list
			while (lCursor != NULL)
			{
				fprintf(fp, ",%d", lCursor->id);
				lCursor = lCursor->next;
			}
		}
		fprintf(fp, "\n");
		cursor = cursor->next;
	} while (cursor != NULL);

	fclose(fp);
}

// prints a line using a given character to the console
void printLine(int len, const char* ch) {
	printf("\n");
	int i;
	for (i = 0; i < len; i++) printf("%s", ch);
	printf("\n");
}


int main(int argc, char* argv[]) {
	CLS;
	HEAD = readGraphFromFile(FILE_NAME);
	// print graph in text format
	printGraph(HEAD);

	int command = 0;
	// display the menue until the user selects exit
	while (1) {
		dispMenu();
		command = getch();
		switch (command)
		{
		case '1':	// add new node
			CLS;
			while (1) {
				printf("Please enter id of the new vertex: ");
				int newId = -1;
				float newX = 0, newY = 0;

				char tmp[20];	// buffer for user input
				scanf("%s", tmp);	// read the input as string
				char *endptr;	// for error checking
				newId = strtol(tmp, &endptr, 10); // do the conversion
				if (endptr == tmp)
				{
					// the user input cannot be converted to integer
					printf("ERR: Your input '%s' is not an integer.\n", tmp);
					continue;
				}
				GNode* foo = getGraphNode(newId);
				if (foo != NULL) {
					printf("ERR: This id='%d' exists in the graph\n", newId);
					continue;
				}

				while (1) {
					printf("Please enter x and y of the vertex %d in following format x,y:", newId);
					int c = scanf(" %f, %f", &newX, &newY);	// read input as string
					if (c != 2) {
						printf("ERR: enter comma separated numbers.\n");
						while (getchar() != '\n');	// clear buffer
						continue;
					}
					else{
						break;	// while for reading x,y
					}
				}
				// perform the action 
				GNode* node = createNode(newId, newX, newY);
				addToEndOfGraphList(HEAD, node);

				// show results
				CLS;
				printf("a new vertex added to graph %d = (%f,%f)", node->id, node->x, node->y);
				printLine(80, "-");
				break;
			}
			break;
		case '2':	// remove a node
			CLS;
			while (1) {
				// dialog
				printf("Please enter id of the vertex to remove: ");
				int nId = -1;
				char tmp[20];	// buffer for user input
				scanf("%s", tmp);	// read the input as string
				char *endptr;	// for error checking
				nId = strtol(tmp, &endptr, 10); // do the conversion
				if (endptr == tmp)
				{
					// the user input cannot be converted to integer
					printf("ERR: Your input '%s' is not an integer.\n", tmp);
					continue;
				}
				// perform the action
				int res = removeNode(nId);
				// show results
				CLS;
				if (res == 0) {
					printf("there was no node with id = %d", nId);
					printLine(80,"-");
				} else {
					printf("the node with id = %d is successfully deleted", nId);
					printLine(80, "-");
				}
				break;
			}
			break;
		case '3':	// add new edge
			CLS;
			while (1) {
				printf("Enter ids of two endpoints of the edge id1,id2: ");
				int id1, id2;
				if (scanf("%d,%d", &id1, &id2) != 2) {
					printf("ERR: Please enter two comma separated integer values.\n");
					while (getchar() != '\n');	// empty buffer for scanf
					continue;
				}
				// perfrom action 
				int res = (id1 == id2) ? 3 : addEdge(id1, id2); 
				// show results
				CLS;
				if (res == 1) printf("first node id is not in the graph\n");
				else if (res == 2) printf("second node id is not in the graph\n");
				else if (res == 3) printf("no edges added. the ids are the same, the graph is not directional.\n");
				else printf("the edge successfully is added to the graph.\n");
				printLine(80, "-"); 
				break;
			}
			break;
		case '4':	// remove an edge
			CLS;
			while (1) {
				// dialog
				printf("Enter ids for two endpoints of the edge (id1,id2): ");
				int id1, id2;
				int c = scanf(" %d,%d", &id1, &id2);
				if (c != 2) {
					printf("ERR: please enter two comma separated integer values.\n");
					while (getchar() != '\n'); //empty buffer for scanf
					continue;
				}
				// perform the action
				int res = (id1 == id2) ? 3 : removeEdge(id1, id2);
				// show results
				CLS;
				if (res == 1) printf("first node id is not in the graph\n");
				else if (res == 2) printf("second node id is not in the graph\n");
				else if (res == 3) printf("the ids are the same. no vertex has an edge to itself.\n");
				else printf("the edge successfully is removed from the graph.\n");
				printLine(80, "-");
				break;
			}
			break;
		case '5': 
			saveGraph(FILE_NAME);
			printf("saved to file\n");
			system("pause");
			return EXIT_SUCCESS;
			break;
		case '6':
			CLS;
			printGraph(HEAD);
			printf("\n");
			printLine(80,"-");
			break;
		default:
			// display error for wrong input
			CLS;
			printf("\nPLEASE ENTER ONE OF THE ACTIONS (1-5)\n");
			printf("input = '%c' is INVALID.\n", command);
			printLine(80, "-");
			break;
		}
	}
}

