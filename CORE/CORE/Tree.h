#pragma once
#include"Misc.h"

using namespace std;
class Label;
class DialogueNode;

class Tree
{
public:

	Tree();

	string GetText(DialogueNode* tr);

	void InsertNode(DialogueNode*& currentNode, DialogueNode*& parentNode, string text, string quest,bool bAnySiblings);

	DialogueNode* m_pRoot;
};


