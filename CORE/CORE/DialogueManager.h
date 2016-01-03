#pragma once
#include"Tree.h"
#include"Label.h"
#include"libs/TinyXml2/tinyxml2.h"
#include "Misc.h"
#include <vector>
#include <string>
#include "GameObjectManager.h"

struct node;

/*
this structure holds all the information needed for single node in the tree
m_nKey - this is the id of the node.
m_strText - the text of the dialogue
m_label - the label is need to detect user click on the text
m_vNodes - this is vector that holds the children of this node.The children can contain children and etc.
m_strQuest - holds the name of the quest
*/
struct DialogueNode
{
	string		  m_strText;
	Label*		  m_pLabel;
	vector<DialogueNode*> m_vNodes;
	string		  m_strQuest;
};

/*
this is struct that holds all the information needed for one dialogue
m_strModel - This variable holds the name of the model. When clicking on this model this dialogue will appear
m_pTree - Holds the ierarchy of dialogue nodes
m_pCurrentDialogueNode - Holds the currender dialogue node, which childs are displayed on the screen
m_pClickedDialogueNode - Holds the dialogue node that is currently selected with mouse
m_bIsClickedDialogueNode - If true we know that some of the dialogue node is clicked. It is used in labelClicked
m_bIsStarted - If true the dilogue is started i.e. the user clicked on the root of the dialogue and can interact
m_bIsEnded - If true the dialogue ended.
*/
struct DialogueObject
{
	string  m_strModel;
	Tree*   m_pTree;
	DialogueNode*   m_pCurrentDialogueNode;
	DialogueNode*   m_pClickedDialogueNode;
	bool    m_bIsClickedDialogueNode;
	bool    m_bIsStarted;
	bool    m_bIsEnded;
};

//This class contains all the functions to load dialogue from XML, 
//change the current dialogue node and render it on the screen
class DialogueManager
{
public:	

	void LoadDialogues(string strDialoguesFileName);

	//void TraverseNodes(Tree* tree, tinyxml2::XMLElement* xmlNode, node* treeNode, node* parentTreeNode, bool childNode, bool siblingNode);

	void TraverseNodes(tinyxml2::XMLElement* xmlNode, Tree* pTree, DialogueNode* currentNode, DialogueNode* parentNode);

	void OnUpdate(map<string, QuestObject>& activeQuests);

	void UpdateLabelTree(DialogueNode* pNode);

	void UpdateLabelTreeRoot(DialogueNode* pNode);

	void RenderLabelTree(DialogueNode* pNode);

	void RenderLabelTreeRoot(DialogueNode* pNode);

	void ChangeDialogue(DialogueNode* pNode,DialogueObject& dialogueObject);

	void LabelClicked(DialogueObject& dialogueObject,map<string,QuestObject>& mapActiveQuests,map<string,QuestObject>& mapAvailableQuests);

	void HideAllLabelTree(DialogueNode* pNode);

	bool AreChildrenHidden(DialogueNode* pNode);

	void AddDialogueObjects(DialogueObject& dialogueObject);

	void StartDialogue(DialogueNode* pNode,DialogueObject& dialogueObject);

	void HideRoot(DialogueNode* pNode);

	map<string,DialogueObject> m_mapModelDialogue;

	vector<string> m_vGameObjectsWithDialogues;

private:

	tinyxml2::XMLElement* m_pRoot;
	tinyxml2::XMLElement* m_pNode;
	tinyxml2::XMLElement* m_pDialogue;

};

extern DialogueManager* pDialogueManager;