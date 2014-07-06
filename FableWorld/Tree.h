#pragma once
#include"Misc.h"

using namespace std;
class Label;

/*
this structure holds all the information needed for single node in the tree
m_nKey - this is the id of the node.
m_strText - the text of the dialogue
m_label - the label is need to detect user click on the text
m_vNodes - this is vector that holds the children of this node.The children can contain children and etc.
m_strQuest - holds the name of the quest
*/
struct node
{
	int			  m_nKey;
	string		  m_strText;
	Label*		  m_pLabel;
	vector<node*> m_vNodes;
	string		  m_strQuest;
};

//This class contains functions for inserting nodes in the tree and searching in the tree
class Tree
{
public:

	Tree();

	string GetText(node* tr);

	node* InsertNode(node* pNode,string text,int val,float x,float y,string quest);

	node* FindNode(node* pNode, int key);

	node* GetRoot();

	void SetRoot(node* pNode);

private:

	node* m_pRoot;
};


