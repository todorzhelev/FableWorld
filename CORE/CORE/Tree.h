#pragma once
#include"Misc.h"


class Label;
class DialogueNode;

class Tree {
public:
	Tree();
	std::string GetText(DialogueNode* tr);
	void InsertNode(DialogueNode*& currentNode, DialogueNode*& parentNode, std::string text, std::string quest,bool bAnySiblings);
	DialogueNode* m_pRoot;
};


