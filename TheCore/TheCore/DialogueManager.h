#pragma once
#include"Tree.h"
#include"Label.h"
#include"tinyxml.h"
#include "Misc.h"
#include <vector>
#include <string>
#include "GameObjectManager.h"

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
	node*   m_pCurrentDialogueNode;
	node*   m_pClickedDialogueNode;
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

	void UpdateLabelTree(node* pNode);

	void UpdateLabelTreeRoot(node* pNode);

	void RenderLabelTree(node* pNode);

	void RenderLabelTreeRoot(node* pNode);

	void ChangeDialogue(node* pNode,DialogueObject& dialogueObject);

	void LabelClicked(DialogueObject& dialogueObject,map<string,QuestObject>& mapActiveQuests,map<string,QuestObject>& mapAvailableQuests);

	void HideAllLabelTree(node* pNode);

	bool AreChildrenHidden(node* pNode);

	void AddDialogueObjects(DialogueObject& dialogueObject);

	void StartDialogue(node* pNode,DialogueObject& dialogueObject);

	void HideRoot(node* pNode);

	map<string,DialogueObject> m_mapModelDialogue;

	vector<string> m_vGameObjectsWithDialogues;

private:

	TiXmlElement* m_pRoot;
	TiXmlElement* m_pLeaf;
	TiXmlElement* m_pDialogue;

};

extern DialogueManager* pDialogueManager;