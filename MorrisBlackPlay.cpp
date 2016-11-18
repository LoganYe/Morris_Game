#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

#define MAX_QUEUE	400

#define W	'W'
#define B	'B'
#define X	'x'

#define OPENING			1
#define MIDGAME_ENDGAME	2

#define USE_ALPHA_BETA		1
#define NOT_USE_ALPHA_BETA	2
int DECISION_TREE_DEPTH	= 4;

using namespace std;
void Copy_data(char *Dst, char *Src, int len);
void Fill_data(char *Dst, char fillData, int len);

void Copy_data(char *Dst, char *Src, int len)
{
    int i;
    for(i=0;i<len;i++)
	{
		if((X != Src[i]) && (B != Src[i]) && (W != Src[i]))		// if wrong data, fill 'x'
			Dst[i] = X;
		else
			Dst[i] = Src[i];
	}
}

void Fill_data(char *Dst, char fillData, int len)
{
    int i;
    for(i=0;i<len;i++)
        Dst[i] = fillData;
}

class queue
{
private:
	long queue_data[MAX_QUEUE];
	short int first;
	short int last;
	
public:
	void enqueue(long dat)
	{
		if(last == first - 1)	// need fix
		{
			printf("queue full change size");
			exit(0);
		}
		
		queue_data[last++] = dat;
		
		if(last >= MAX_QUEUE)
		{ last = 0; }
	}
	
	long dequeue(void)
	{
		long ret = 0;
		
		if(last == first)	// need fix
		{
			printf("queue already empty");
			exit(0);
		}
		
		ret = queue_data[first++];
		
		if(first >= MAX_QUEUE)
		{ first = 0; }
		
		return ret;
	}
	
	queue()
	{
		first = 0;
		last = 0;
	}
};

class Node
{
public:
	char position[23];
	int depth;
	int staticEstimationValue;
	Node *child;
	Node *sibling;
	Node *father;
};

class Tree
{
public:
	Node *root;

	Tree()
	{
		root = NULL;
	}

	Tree(char pos[23])
	{
		root = new Node();
		Copy_data(root->position, pos, 23);
		root->child = NULL;
		root->sibling = NULL;
		root->father = root;
		root->depth = 0;
	}
	
	Node *insertChild(Node *n, char pos[23]);
	Node *insertSibling(Node *n, char pos[23]);
	
	int ListSize(Node *s);
	
	Node *rootNode();
	
	void displayList(Node *s);
	void showSibling(Node *start);
	void showChild(Node *start);

	void deleteWholeTree(Node *start);
};

Node *Tree::rootNode()
{
	return (root);
}

Node *Tree::insertSibling(Node *n, char pos[23])
{
	Node *tmp = n->sibling;
	n->sibling = new Node();
	Copy_data(n->sibling->position, pos, 23);
	n->sibling->child = NULL;
	n->sibling->sibling = tmp;
	n->sibling->father = n->father;
	n->sibling->depth = n->depth;
	return (n);
}

Node *Tree::insertChild(Node *n, char pos[23])
{
	if(NULL == n)	// no root data, just create root Node
	{
		Node *tmp = new Node();
		root = tmp;
		Copy_data(tmp->position, pos, 23);
		tmp->child = NULL;
		tmp->sibling = NULL;
		tmp->father = tmp;
		tmp->depth = 0;
		return (tmp);
	}

	if(n->child != NULL)	// already had child, add in child->child
	{
		insertChild(n->child, pos);
	}
	else	// no child, add
	{
		n->child = new Node();
		Copy_data(n->child->position, pos, 23);
		n->child->child = NULL;
		n->child->sibling = NULL;
		n->child->father = n;
		n->child->depth = n->depth + 1;
	}

	return (n);
}

void Tree::displayList(Node *start)
{
	int i;
	Node *tmp;
	
	tmp = start;
	
	while(tmp != NULL)	// show child
	{
		cout << "->" << endl;
		for(i=0;i<23;i++)
			cout << tmp->position[i];
		cout << " Depth:" << tmp->depth;

		if((tmp->sibling) != NULL)
			showSibling(tmp->sibling);
		cout << "\r\n";
		tmp = tmp->child;
	}

}

void Tree::showChild(Node *start)
{
	int i;
	Node *tmp;
	tmp = start;
	
	while(tmp != NULL)	// show child
	{
		cout << "->[" << endl;
		for(i=0;i<23;i++)
			cout << tmp->position[i];
		cout << " Depth:" << tmp->depth;

		if((tmp->sibling) != NULL)
			showSibling(tmp->sibling);
		cout << "]";
		tmp = tmp->child;
	}

}

void Tree::showSibling(Node *start)
{
	int i;
	Node *tmp;
	tmp = start;
	
	while(tmp != NULL)
	{
		cout << ">>";
		for(i=0;i<23;i++)
			cout << tmp->position[i];
		cout << " Depth:" << tmp->depth;

		if((tmp->child) != NULL)
			showChild(tmp->child);
		tmp = tmp->sibling;
	}
}

// recursively delete memory used by the tree
void Tree::deleteWholeTree(Node *start)
{
	if(start->sibling != NULL)	// delete siblings
	{
		deleteWholeTree(start->sibling);
		start->sibling = NULL;
	}
	if(start->child != NULL)	// delete child
	{
		deleteWholeTree(start->child);
		start->child = NULL;
	}
	delete start;
}

class List
{
public:
	Node *listHead;
	int depth;
	Node *father;

	List()
	{
		listHead = NULL;
		depth = -1;
		father = NULL;
	}

	List(int fatherDepth, Node *fatherNode)
	{
		listHead = NULL;
		depth = fatherDepth + 1;
		father = fatherNode;
	}

	void insertSibling(char pos[23]);

	void freshWholeList(Node *start, Node *ref);
	Node *linkUnderNode(Node *n);
	Node *headNode();

};

Node *List::headNode()
{
	return (listHead);
}

void List::freshWholeList(Node *start, Node *ref)
{
	while(start != NULL)
	{
		start->father = ref;
		start->depth = ref->depth + 1;
		start = start->sibling;
	}
}

Node *List::linkUnderNode(Node *n)
{
	if(NULL == n->child)
	{
		n->child = listHead;

		if((listHead->father == n) && (listHead->depth == n->depth + 1))	// father & depth were correct
		{
		}
		else	// if not correct
		{
			freshWholeList(listHead, n);
		}
	}
}

void List::insertSibling(char pos[23])
{
	if(NULL == listHead)	// input first one
	{
		listHead = new Node();
		Copy_data(listHead->position, pos, 23);
		listHead->child = NULL;
		listHead->sibling = NULL;
		if(depth != -1)		// already know father & depth
		{
			listHead->father = father;
			listHead->depth = depth;
		}
	}
	else	// input sibling
	{
		Node *tmp = listHead->sibling;
		listHead->sibling = new Node();
		Copy_data(listHead->sibling->position, pos, 23);
		listHead->sibling->child = NULL;
		listHead->sibling->sibling = tmp;
		listHead->sibling->father = listHead->father;
		listHead->sibling->depth = listHead->depth;
	}
}


class Morris
{
public:
	void GenerateMovesOpening(char In_pos[23], Node *CurNode, int isAlpha);
	void GenerateMovesMidgameEndgame(char In_pos[23], Node *CurNode, int isAlpha);
	void GenerateOpeningTree(char In_pos[23], Node *CurNode, int depth, int whiteCnt, int blackCnt);
	int GenerateMidgameEndgameTree(char In_pos[23], Node *CurNode, int depth);

	void GenerateAddWhitePlay(char In_pos[23], Node *CurNode);
	void GenerateRemoveWhitePlay(char In_pos[23], List *CurList);
	void GenerateHoppingWhitePlay(char In_pos[23], Node *CurNode);
	void GenerateMoveWhitePlay(char In_pos[23], Node *CurNode);
	void GenerateAddBlackPlay(char In_pos[23], Node *CurNode);
	void GenerateRemoveBlackPlay(char In_pos[23], List *CurList);
	void GenerateHoppingBlackPlay(char In_pos[23], Node *CurNode);
	void GenerateMoveBlackPlay(char In_pos[23], Node *CurNode);

	int estimationOpening(Node *CurNode);
	int estimationMidgameEndgame(Node *CurNode);
	int staticEstimation(Node *CurNode, int mode);

	int WhitePiecesNum(char pos[23]);
	int BlackPiecesNum(char pos[23]);

	void neighborPos(int location, char neighbor[5]);	// neighbor's first BYTE indicate how many neighbors
	int closeMill(int location, char pos[23]);

	int Max(int a, int b);
	int Min(int a, int b);

	int MaxMin(Node *CurNode, int mode);
	int MinMax(Node *CurNode, int mode);

	int alpha_MaxMin(Node *CurNode, int mode, int alpha, int beta);
	int alpha_MinMax(Node *CurNode, int mode, int alpha, int beta);

	Node *FindMaxSibling(Node *CurNode);
	void OutputResult(Node *CurNode, char *OutputFileName, int mode);
};

void Morris::neighborPos(int location, char neighbor[5])
{
	switch(location)
	{
	case 0:
		neighbor[0] = 3; neighbor[1] = 1; neighbor[2] = 3; neighbor[3] = 8;
		break;
	case 1:
		neighbor[0] = 3; neighbor[1] = 0; neighbor[2] = 2; neighbor[3] = 4;
		break;
	case 2:
		neighbor[0] = 3; neighbor[1] = 1; neighbor[2] = 5; neighbor[3] = 13;
		break;
	case 3:
		neighbor[0] = 4; neighbor[1] = 0; neighbor[2] = 4; neighbor[3] = 6; neighbor[4] = 9;
		break;
	case 4:
		neighbor[0] = 3; neighbor[1] = 1; neighbor[2] = 3; neighbor[3] = 5;
		break;
	case 5:
		neighbor[0] = 4; neighbor[1] = 2; neighbor[2] = 4; neighbor[3] = 7; neighbor[4] = 12;
		break;
	case 6:
		neighbor[0] = 3; neighbor[1] = 3; neighbor[2] = 7; neighbor[3] = 10;
		break;
	case 7:
		neighbor[0] = 3; neighbor[1] = 5; neighbor[2] = 6; neighbor[3] = 11;
		break;
	case 8:
		neighbor[0] = 3; neighbor[1] = 0; neighbor[2] = 9; neighbor[3] = 20;
		break;
	case 9:
		neighbor[0] = 4; neighbor[1] = 3; neighbor[2] = 8; neighbor[3] = 10; neighbor[4] = 17;
		break;
	case 10:
		neighbor[0] = 3; neighbor[1] = 6; neighbor[2] = 9; neighbor[3] = 14;
		break;
	case 11:
		neighbor[0] = 3; neighbor[1] = 7; neighbor[2] = 12; neighbor[3] = 16;
		break;
	case 12:
		neighbor[0] = 4; neighbor[1] = 5; neighbor[2] = 11; neighbor[3] = 13; neighbor[4] = 19;
		break;
	case 13:
		neighbor[0] = 3; neighbor[1] = 2; neighbor[2] = 12; neighbor[3] = 22;
		break;
	case 14:
		neighbor[0] = 3; neighbor[1] = 10; neighbor[2] = 15; neighbor[3] = 17;
		break;
	case 15:
		neighbor[0] = 3; neighbor[1] = 14; neighbor[2] = 16; neighbor[3] = 18;
		break;
	case 16:
		neighbor[0] = 3; neighbor[1] = 11; neighbor[2] = 15; neighbor[3] = 19;
		break;
	case 17:
		neighbor[0] = 4; neighbor[1] = 9; neighbor[2] = 14; neighbor[3] = 18; neighbor[4] = 20;
		break;
	case 18:
		neighbor[0] = 4; neighbor[1] = 15; neighbor[2] = 17; neighbor[3] = 19; neighbor[4] = 21;
		break;
	case 19:
		neighbor[0] = 4; neighbor[1] = 12; neighbor[2] = 16; neighbor[3] = 18; neighbor[4] = 22;
		break;
	case 20:
		neighbor[0] = 3; neighbor[1] = 8; neighbor[2] = 17; neighbor[3] = 21;
		break;
	case 21:
		neighbor[0] = 3; neighbor[1] = 18; neighbor[2] = 20; neighbor[3] = 22;
		break;
	case 22:
		neighbor[0] = 3; neighbor[1] = 13; neighbor[2] = 19; neighbor[3] = 21;
		break;
	}
}

int Morris::closeMill(int location, char pos[23])
{
	char C = pos[location];
	if((C != W) && (C != B))
		return 0;	// C must be W or B

	switch(location)
	{
		case 0:
			if(((C == pos[1]) && (C == pos[2])) || ((C == pos[3]) && (C == pos[6])) || ((C == pos[8]) && (C ==
					pos[20])))
				return 1;
			break;
		case 1:
			if(((C == pos[0]) && (C == pos[2])))
				return 1;
			break;
		case 2:
			if(((C == pos[0]) && (C == pos[1])) || ((C == pos[5]) && (C == pos[7])) || ((C == pos[13]) && (C ==
					pos[22])))
				return 1;
			break;
		case 3:
			if(((C == pos[0]) && (C == pos[6])) || ((C == pos[4]) && (C == pos[5])) || ((C == pos[9]) && (C ==
					pos[17])))
				return 1;
			break;
		case 4:
			if(((C == pos[3]) && (C == pos[5])))
				return 1;
			break;
		case 5:
			if(((C == pos[3]) && (C == pos[4])) || ((C == pos[7]) && (C == pos[2])) || ((C == pos[12]) && (C ==
					pos[19])))
				return 1;
			break;
		case 6:
			if(((C == pos[10]) && (C == pos[14])) || ((C == pos[3]) && (C == pos[0])))
				return 1;
			break;
		case 7:
			if(((C == pos[5]) && (C == pos[2])) || ((C == pos[11]) && (C == pos[16])))
				return 1;
			break;
		case 8:
			if(((C == pos[0]) && (C == pos[20])) || ((C == pos[9]) && (C == pos[10])))
				return 1;
			break;
		case 9:
			if(((C == pos[8]) && (C == pos[10])) || ((C == pos[3]) && (C == pos[17])))
				return 1;
			break;
		case 10:
			if(((C == pos[8]) && (C == pos[9])) || ((C == pos[6]) && (C == pos[14])))
				return 1;
			break;
		case 11:
			if(((C == pos[12]) && (C == pos[13])) || ((C == pos[7]) && (C == pos[16])))
				return 1;
			break;
		case 12:
			if(((C == pos[11]) && (C == pos[13])) || ((C == pos[5]) && (C == pos[19])))
				return 1;
			break;
		case 13:
			if(((C == pos[11]) && (C == pos[12])) || ((C == pos[2]) && (C == pos[22])))
				return 1;
			break;
		case 14:
			if(((C == pos[15]) && (C == pos[16])) || ((C == pos[6]) && (C == pos[10])) || ((C == pos[17]) && (C == pos[20])))
				return 1;
			break;
		case 15:
			if(((C == pos[14]) && (C == pos[16])) || ((C == pos[18]) && (C == pos[21])))
				return 1;
			break;
		case 16:
			if(((C == pos[14]) && (C == pos[15])) || ((C == pos[7]) && (C == pos[11])) || ((C == pos[19]) && (C == pos[22])))
				return 1;
			break;
		case 17:
			if(((C == pos[18]) && (C == pos[19])) || ((C == pos[3]) && (C == pos[9])) || ((C == pos[14]) && (C == pos[20])))
				return 1;
			break;
		case 18:
			if(((C == pos[17]) && (C == pos[19])) || ((C == pos[15]) && (C == pos[21])))
				return 1;
			break;
		case 19:
			if(((C == pos[17]) && (C == pos[18])) || ((C == pos[5]) && (C == pos[12])) || ((C == pos[16]) && (C == pos[22])))
				return 1;
			break;
		case 20:
			if(((C == pos[21]) && (C == pos[22])) || ((C == pos[0]) && (C == pos[8])) || ((C == pos[14]) && (C == pos[17])))
				return 1;
			break;
		case 21:
			if(((C == pos[20]) && (C == pos[22])) || ((C == pos[15]) && (C == pos[18])))
				return 1;
			break;
		case 22:
			if(((C == pos[20]) && (C == pos[21])) || ((C == pos[2]) && (C == pos[13])) || ((C == pos[16]) && (C == pos[19])))
				return 1;
			break;
		default:
			break;
	}
	return 0;
}

int Morris::WhitePiecesNum(char pos[23])
{
	// 1:return 1, 2:return 2, 3:return 3, More than 4:return 4
	int count = 0;
	int i;
	for(i=0;i<23;i++)
	{
		if(W == pos[i])
			count++;
		if(count >=4)
			return 4;
	}
	return count;
}

int Morris::BlackPiecesNum(char pos[23])
{
	// 1:return 1, 2:return 2, 3:return 3, More than 4:return 4
	int count = 0;
	int i;
	for(i=0;i<23;i++)
	{
		if(B == pos[i])
			count++;
		if(count >=4)
			return 4;
	}
	return count;
}

Node *Morris::FindMaxSibling(Node *CurNode)
{
	Node *newNode;
	int maxValue = -1000000;

	if(NULL == CurNode)
		return NULL;

	newNode = CurNode;
	while(newNode != NULL)
	{
		if(newNode->staticEstimationValue > maxValue)
			maxValue = newNode->staticEstimationValue;

		newNode = newNode->sibling;
	}

	newNode = CurNode;
	while(newNode != NULL)
	{
		if(maxValue == newNode->staticEstimationValue)
			return newNode;

		newNode = newNode->sibling;
	}
}

void Morris::OutputResult(Node *CurNode, char *OutputFileName, int mode)
{
	Node *newNode;
	int j;
	// output file
	ofstream ocout;

	ocout.open(OutputFileName);

	newNode = FindMaxSibling(CurNode->child);
	if (NULL == newNode)
		return;

	cout << "Board Position:";
	for (j = 0; j < 23; j++)
		cout << newNode->position[j];
	cout << "." << endl;

	ocout << "Board Position:";
	for (j = 0; j < 23; j++)
		ocout << newNode->position[j];
	ocout << "." << endl;

	ocout << "Positions evaluated by static estimation:" << staticEstimation(newNode, mode) << "." << endl;
	ocout << "MINIMAX estimate:" << newNode->staticEstimationValue << "." << endl;

}

void Morris::GenerateMovesOpening(char In_pos[23], Node *CurNode, int isAlpha)
{
	Node *newNode;
	int i=0;
	int j;
	int whiteCnt = 0;
	int blackCnt = 0;

	for(i=0;i<23;i++)
	{
		if(W == In_pos[i])
			whiteCnt++;
		else if(B == In_pos[i])
			blackCnt++;
	}
	GenerateOpeningTree(In_pos, CurNode, 1, whiteCnt, blackCnt);

	newNode = CurNode->child;
	i = 0;
	while(newNode != NULL)
	{
		i++;
		if(isAlpha != USE_ALPHA_BETA)
			newNode->staticEstimationValue = MinMax(newNode, OPENING);
		else
			newNode->staticEstimationValue = alpha_MinMax(newNode, OPENING, -1000000, 1000000);

		for(j=0;j<23;j++)
			cout << newNode->position[j];
		cout << "No." << i << " Est Val=" << newNode->staticEstimationValue << ", " << endl;

		newNode = newNode->sibling;
	}
}

void Morris::GenerateOpeningTree(char In_pos[23], Node *CurNode, int depth, int whiteCnt, int blackCnt)
{
	Node *newNode;

	if(depth > DECISION_TREE_DEPTH)
		return;

	if((whiteCnt > 9) || (blackCnt >9))
		return;

	if(0 == depth % 2)
	{
		GenerateAddWhitePlay(In_pos, CurNode);

		newNode = CurNode->child;
		while(newNode != NULL)
		{
			GenerateOpeningTree(newNode->position, newNode, depth + 1, whiteCnt + 1, blackCnt);
			newNode = newNode->sibling;
		}
	}
	else
	{
		GenerateAddBlackPlay(In_pos, CurNode);

		newNode = CurNode->child;
		while(newNode != NULL)
		{
			GenerateOpeningTree(newNode->position, newNode, depth + 1, whiteCnt, blackCnt + 1);
			newNode = newNode->sibling;
		}
	}


}

void Morris::GenerateMovesMidgameEndgame(char In_pos[23], Node *CurNode, int isAlpha)
{
	Node *newNode;
	int i=0;
	int j;
	GenerateMidgameEndgameTree(In_pos, CurNode, 1);

	newNode = CurNode->child;
	while(newNode != NULL)
	{
		i++;
		if(isAlpha != USE_ALPHA_BETA)
			newNode->staticEstimationValue = MinMax(newNode, MIDGAME_ENDGAME);
		else
			newNode->staticEstimationValue = alpha_MinMax(newNode, MIDGAME_ENDGAME, -1000000, 1000000);
		for(j=0;j<23;j++)
			cout << newNode->position[j];
		cout << "No." << i << " Est Val=" << newNode->staticEstimationValue << ", " << endl;

		newNode = newNode->sibling;
	}
}

int Morris::GenerateMidgameEndgameTree(char In_pos[23], Node *CurNode, int depth)
{
	Node *newNode;
	int numWhitePieces;
	int numBlackPieces;
	int ret;

	if (depth > DECISION_TREE_DEPTH)
		return 0;


	if (0 == depth % 2)		// White play
	{
		numWhitePieces = WhitePiecesNum(In_pos);
		if(numWhitePieces <= 2)
			return 0;
		else if(3 == numWhitePieces)
			GenerateHoppingWhitePlay(In_pos, CurNode);
		else
			GenerateMoveWhitePlay(In_pos, CurNode);
	}
	else
	{
		numBlackPieces = BlackPiecesNum(In_pos);
		if(numBlackPieces <= 2)
			return 0;
		else if(3 == numBlackPieces)
			GenerateHoppingBlackPlay(In_pos, CurNode);
		else
			GenerateMoveBlackPlay(In_pos, CurNode);
	}

	newNode = CurNode->child;
	while(newNode != NULL)
	{
		ret = GenerateMidgameEndgameTree(newNode->position, newNode, depth + 1);


		newNode = newNode->sibling;
	}

	return 1;
}

void Morris::GenerateAddWhitePlay(char In_pos[23], Node *CurNode)
{
	int i;
	char position[23];
	List newlist(CurNode->depth, CurNode->father);

	for(i=0;i<23;i++)
	{
		if(X == In_pos[i])
		{
			Copy_data(position, In_pos, 23);	// copy of board
			position[i] = W;
			if(closeMill(i, position))
			{
				GenerateRemoveWhitePlay(position, &newlist);
			}
			else
			{
				newlist.insertSibling(position);	// add board position into List
			}
		}
	}

	newlist.linkUnderNode(CurNode);		// attach board position List under CurNode
}

void Morris::GenerateAddBlackPlay(char In_pos[23], Node *CurNode)
{
	int i;
	char position[23];
	List newlist(CurNode->depth, CurNode->father);

	for(i=0;i<23;i++)
	{
		if(X == In_pos[i])
		{
			Copy_data(position, In_pos, 23);	// copy of board
			position[i] = B;
			if(closeMill(i, position))
			{
				GenerateRemoveBlackPlay(position, &newlist);
			}
			else
			{
				newlist.insertSibling(position);	// add board position into List
			}
		}
	}

	newlist.linkUnderNode(CurNode);		// attach board position List under CurNode
}

void Morris::GenerateRemoveWhitePlay(char In_pos[23], List *CurList)
{
	int i;
	int count = 0;
	char position[23];
	for(i=0;i<23;i++)
	{
		if( B == In_pos[i])
		{
			if(!closeMill(i,In_pos))
			{
				Copy_data(position, In_pos, 23);	// copy of board
				position[i] = X;
				CurList->insertSibling(position);	// add board position into List
				count++;

			}
		}
	}
	if(0 == count)	// all black pieces are in mills
		CurList->insertSibling(In_pos);	// add board position into List

}

void Morris::GenerateRemoveBlackPlay(char In_pos[23], List *CurList)
{
	int i;
	int count = 0;
	char position[23];
	for(i=0;i<23;i++)
	{
		if( W == In_pos[i])
		{
			if(!closeMill(i,In_pos))
			{
				Copy_data(position, In_pos, 23);	// copy of board
				position[i] = X;
				CurList->insertSibling(position);	// add board position into List
				count++;

			}
		}
	}
	if(0 == count)	// all black pieces are in mills
		CurList->insertSibling(In_pos);	// add board position into List

}

void Morris::GenerateHoppingWhitePlay(char In_pos[23], Node *CurNode)
{
	int i,j;
	char position[23];
	List newlist(CurNode->depth, CurNode->father);

	for(i=0;i<23;i++)
	{
		if(W == In_pos[i])
		{
			for(j=0;j<23;j++)
			{
				if(X == In_pos[j])
				{
					Copy_data(position, In_pos, 23);    // copy of board
					position[i] = X;
					position[j] = W;
					if (closeMill(j, position))
					{
						GenerateRemoveWhitePlay(position, &newlist);
					}
					else
					{
						newlist.insertSibling(position);    // add board position into List
					}
				}
			}
		}
	}
	newlist.linkUnderNode(CurNode);		// attach board position List under CurNode
}

void Morris::GenerateHoppingBlackPlay(char In_pos[23], Node *CurNode)
{
	int i,j;
	char position[23];
	List newlist(CurNode->depth, CurNode->father);

	for(i=0;i<23;i++)
	{
		if(B == In_pos[i])
		{
			for(j=0;j<23;j++)
			{
				if(X == In_pos[j])
				{
					Copy_data(position, In_pos, 23);    // copy of board
					position[i] = X;
					position[j] = B;
					if (closeMill(j, position))
					{
						GenerateRemoveBlackPlay(position, &newlist);
					}
					else
					{
						newlist.insertSibling(position);    // add board position into List
					}
				}
			}
		}
	}
	newlist.linkUnderNode(CurNode);		// attach board position List under CurNode
}

void Morris::GenerateMoveWhitePlay(char In_pos[23], Node *CurNode)
{
	int i,j;
	char position[23];
	char neighbor[5];	// store neighbor data
	List newlist(CurNode->depth, CurNode->father);

	for(i=0;i<23;i++)
	{
		if(W == In_pos[i])
		{
			neighborPos(i,neighbor);
			for(j=0;j<neighbor[0];j++)
			{
				if(X == In_pos[neighbor[j+1]])
				{
					Copy_data(position, In_pos, 23);    // copy of board
					position[i] = X;
					position[neighbor[j+1]] = W;
					if (closeMill(neighbor[j+1], position))
					{
						GenerateRemoveWhitePlay(position, &newlist);
					}
					else
					{
						newlist.insertSibling(position);    // add board position into List
					}
				}
			}
		}
	}
	newlist.linkUnderNode(CurNode);		// attach board position List under CurNode
}

void Morris::GenerateMoveBlackPlay(char In_pos[23], Node *CurNode)
{
	int i,j;
	char position[23];
	char neighbor[5];	// store neighbor data
	List newlist(CurNode->depth, CurNode->father);

	for(i=0;i<23;i++)
	{
		if(B == In_pos[i])
		{
			neighborPos(i,neighbor);
			for(j=0;j<neighbor[0];j++)
			{
				if(X == In_pos[neighbor[j+1]])
				{
					Copy_data(position, In_pos, 23);    // copy of board
					position[i] = X;
					position[neighbor[j+1]] = B;
					if (closeMill(neighbor[j+1], position))
					{
						GenerateRemoveBlackPlay(position, &newlist);
					}
					else
					{
						newlist.insertSibling(position);    // add board position into List
					}
				}
			}
		}
	}
	newlist.linkUnderNode(CurNode);		// attach board position List under CurNode
}

int Morris::estimationOpening(Node *CurNode)
{
	int numWhitePieces = 0;
	int numBlackPieces = 0;
	int i;

	for(i=0;i<23;i++)
	{
		if(W == CurNode->position[i])
			numWhitePieces++;
		else if(B == CurNode->position[i])
			numBlackPieces++;
	}
	return (numBlackPieces - numWhitePieces);
}

int Morris::estimationMidgameEndgame(Node *CurNode)
{
	int numWhitePieces = 0;
	int numBlackPieces = 0;
	int numWhiteMoves = 0;	// ???
	char neighbor[5];
	int i,j;

	for(i=0;i<23;i++)
	{
		if(W == CurNode->position[i])
			{
				numWhitePieces++;
				for(j=0;j<23;j++)
				{
					// ???
					if(X == CurNode->position[j])
						numWhiteMoves++;
				}
			}
		else if(B == CurNode->position[i])
		{
			numBlackPieces++;
			
		}
	}

	if(numBlackPieces <= 2)
		return (-10000);
	else if(numWhitePieces <= 2)
		return (10000);
	else if(0 == numWhiteMoves)
		return (10000);
	else
		return ((numBlackPieces - numWhitePieces)*1000 - numWhiteMoves);
}

int Morris::staticEstimation(Node *CurNode, int mode)
{
	if(OPENING == mode)
		return estimationOpening(CurNode);
	else if(MIDGAME_ENDGAME == mode)
		return estimationMidgameEndgame(CurNode);
}

int Morris::Max(int a, int b)
{
	if(a > b)
		return a;
	else
		return b;
}

int Morris::Min(int a, int b)
{
	if(a < b)
		return a;
	else
		return b;
}

int Morris::MaxMin(Node *CurNode, int mode)
{
	Node *newNode;
	int v;

	if(NULL == CurNode->child)
		return staticEstimation(CurNode, mode);
	else
	{
		v = -1000000;
		newNode = CurNode->child;

		while(newNode != NULL)
		{
			v = Max(v, MinMax(newNode, mode));
			newNode = newNode->sibling;
		}
	}
	return v;
}

int Morris::MinMax(Node *CurNode, int mode)
{
	Node *newNode;
	int v;

	if(NULL == CurNode->child)
		return staticEstimation(CurNode, mode);
	else
	{
		v = 1000000;
		newNode = CurNode->child;

		while(newNode != NULL)
		{
			v = Min(v, MaxMin(newNode, mode));
			newNode = newNode->sibling;
		}
	}
	return v;
}

int Morris::alpha_MaxMin(Node *CurNode, int mode, int alpha, int beta)
{
	Node *newNode;
	int v;

	if(NULL == CurNode->child)
		return staticEstimation(CurNode, mode);
	else
	{
		v = -1000000;
		newNode = CurNode->child;

		while(newNode != NULL)
		{
			v = Max(v, alpha_MinMax(newNode, mode, alpha, beta));
			if(v >= beta)
				return v;
			else
				alpha = max(v, alpha);
			newNode = newNode->sibling;
		}
	}
	return v;
}

int Morris::alpha_MinMax(Node *CurNode, int mode, int alpha, int beta)
{
	Node *newNode;
	int v;

	if(NULL == CurNode->child)
		return staticEstimation(CurNode, mode);
	else
	{
		v = +1000000;
		newNode = CurNode->child;

		while(newNode != NULL)
		{
			v = Min(v, alpha_MaxMin(newNode, mode, alpha, beta));
			if(v <= alpha)
				return v;
			else
				beta = min(v, beta);
			newNode = newNode->sibling;
		}
	}
	return v;
}

/*
 *
 */

int main(int argc, char *argv[])
{
	int tst;
	int i;
//	char InputArray[23] = {B,X,B,B,W,W,W,B,X,X,W,B,W};
	char InputArray[23] = {B,X,B,X,W,W,W,X,X,X,X,X,B};

	string inputFileName;
	string outputFileName;

	if(4 == argc)	// file input output
	{
		// input file
		ifstream fcin;

		fcin.open(argv[1]);
		for(i=0;i<23;i++)
		{
			fcin >> InputArray[i];
		}

		DECISION_TREE_DEPTH = *argv[3] - '0';

		printf("depth:%d\r\n",DECISION_TREE_DEPTH);

		Tree *DecideTree = new Tree(InputArray);
		Node *root = DecideTree->rootNode();
		Morris *MorrisPlay = new Morris();


//		MorrisPlay->GenerateMovesOpening(root->position, root, USE_ALPHA_BETA);		// Opening
//		MorrisPlay->OutputResult(root, argv[2], OPENING);
		MorrisPlay->GenerateMovesMidgameEndgame(root->position, root, USE_ALPHA_BETA);        // MidGameEndGame
		MorrisPlay->OutputResult(root, argv[2], MIDGAME_ENDGAME);

		DecideTree->deleteWholeTree(root);    // free the memory

	}
	else
	{
		DECISION_TREE_DEPTH = 4;
		Tree *DecideTree = new Tree(InputArray);
		Node *root = DecideTree->rootNode();
		Morris *MorrisPlay = new Morris();

		MorrisPlay->GenerateMovesOpening(root->position, root, USE_ALPHA_BETA);		// Opening
//		MorrisPlay->GenerateMovesMidgameEndgame(root->position, root, USE_ALPHA_BETA);        // MidGameEndGame

		DecideTree->deleteWholeTree(root);    // free the memory
	}

	cin >> tst;
	return 0;
}

