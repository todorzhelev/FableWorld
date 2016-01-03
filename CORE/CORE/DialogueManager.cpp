#include "DialogueManager.h"
#include "SkinnedModel.h"
#include "LuaFunctions.h"

DialogueManager* pDialogueManager = NULL;

//TODO: this code should be completly refactored

/*
SourceFile: Dialogue.cpp
 This file include the definitions of the functions needed to load dialogue render and update it.
		 The purpose of the dialogue is the player to receive quest depending on his decisions through the dialogue.
		 The dialogue nodes are displayed as text one below another and the user can follow the dialogue by clicking on the desired dialogue
		 The dialogue nodes are organised as a tree of GuiLabels.A class that simulates tree is implemented - in Tree.cpp 
		 and for detecting whether user is clicked on some of the text is used the GuiLabel class.
*/

/////////////////////////////////////////////////////////////////////////

void DialogueManager::LoadDialogues(string strDialoguesFileName)
{
	tinyxml2::XMLDocument doc;
	//loads the xml file
	tinyxml2::XMLError err = doc.LoadFile(strDialoguesFileName.c_str());

	if( err != tinyxml2::XMLError::XML_SUCCESS )
	{
		MessageBox(0,"cant load xml file",0,0);
	}

	//gets the root element of the whole xml document- Dialogues
	m_pRoot = doc.FirstChildElement("Dialogue");
	if(m_pRoot)
	{
		//gets the root element of single dialogue
		m_pDialogue = m_pRoot;
		while(m_pDialogue)
		{
			DialogueObject obj;
			obj.m_pTree = new Tree;
			obj.m_strModel = m_pDialogue->Attribute("model");
			m_vGameObjectsWithDialogues.push_back(obj.m_strModel);
		
			m_pNode = m_pDialogue->FirstChildElement("Node");
		
			TraverseNodes(m_pNode, obj.m_pTree, obj.m_pTree->m_pRoot,nullptr);

			//when the dialogue is rendered and updated it is passed the current node from it.
			//This dialogue isnt started yet so the current node is the root
			obj.m_pCurrentDialogueNode = obj.m_pTree->m_pRoot;

			obj.m_bIsStarted = false;
			obj.m_bIsClickedDialogueNode = false;
			obj.m_bIsEnded = false;

			//adds the loaded from xml dialogue to map
			AddDialogueObjects(obj);

			//goes to the next Dialogue element
			m_pDialogue = m_pDialogue->NextSiblingElement("Dialogue");
		}
	}
}

/////////////////////////////////////////////////////////////////////////

//void DialogueManager::TraverseNodes(Tree* tree,tinyxml2::XMLElement* xmlNode, DialogueNode* treeNode, DialogueNode* parentTreeNode, bool childNode,bool siblingNode)
void DialogueManager::TraverseNodes(tinyxml2::XMLElement* xmlNode, Tree* pTree, DialogueNode* currentNode, DialogueNode* parentNode)
{
	if(xmlNode == nullptr )
	{
		return;
	}
	
	string quest = "";
	string text = "";

	if (xmlNode->Attribute("Quest"))
	{
		quest = xmlNode->Attribute("Quest");
	}

	if (xmlNode->Attribute("Text"))
	{
		text = xmlNode->Attribute("Text");
	}

	tinyxml2::XMLElement* pSibling = xmlNode->NextSiblingElement("Node");
	tinyxml2::XMLElement* pFirstChild = xmlNode->FirstChildElement("Node");

	tinyxml2::XMLElement* pPrevSibling = xmlNode->PreviousSiblingElement("Node");

	bool bAnySiblings = pSibling != nullptr || pPrevSibling != nullptr;

	if( currentNode == nullptr && parentNode == nullptr )
	{
		pTree->InsertNode(pTree->m_pRoot, parentNode, text, quest, bAnySiblings);
		currentNode = pTree->m_pRoot;
	}
	else
	{
		pTree->InsertNode(currentNode, parentNode, text, quest, bAnySiblings);
	}

	if (pFirstChild != nullptr)
	{
		TraverseNodes(pFirstChild,pTree, nullptr,currentNode);
	}

	if (pSibling != nullptr)
	{
		TraverseNodes(pSibling,pTree,nullptr, parentNode);
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::OnUpdate(map<string, QuestObject>& activeQuests)
{
	for (auto& dialogue : m_mapModelDialogue)
	{
		if (!dialogue.second.m_bIsEnded)
		{
			pDialogueManager->UpdateLabelTree(dialogue.second.m_pTree->m_pRoot);
			pDialogueManager->UpdateLabelTreeRoot(dialogue.second.m_pTree->m_pRoot);
			pDialogueManager->StartDialogue(dialogue.second.m_pTree->m_pRoot, dialogue.second);
			pDialogueManager->ChangeDialogue(dialogue.second.m_pCurrentDialogueNode, dialogue.second);
			pDialogueManager->LabelClicked(dialogue.second, activeQuests, availableQuests);
		}

		GameObject* obj = m_pGameObjManager->GetObjectByName(dialogue.second.m_strModel);

		if (obj->IsPicked() && !dialogue.second.m_bIsEnded)
		{
			dialogue.second.m_pTree->m_pRoot->m_pLabel->SetVisible(true);
			obj->SetPicked(false);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/*
add the passed dialogue object to modelDialogue map.
This map connects actual model in game(dwarf,robot, etc) with dialogue 
so when the user clicks on the model the dialogue is displayed
*/
void DialogueManager::AddDialogueObjects(DialogueObject& dialogueObject)
{
	m_mapModelDialogue[dialogueObject.m_strModel] = dialogueObject; 
}

/////////////////////////////////////////////////////////////////////////
/*
invokes the onUpdate function for every label in the tree
This is needed to check for mouse click or mouse over
*/
void DialogueManager::UpdateLabelTree(DialogueNode* pNode)
{
	if(pNode == NULL)
	{
		return;	
	}

	for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
	{
		pNode->m_vNodes[i]->m_pLabel->OnUpdate();
		UpdateLabelTree(pNode->m_vNodes[i]);
	}
}

/////////////////////////////////////////////////////////////////////////
/*
invokes the onUpdate function for the root element in the tree
This is needed to check for mouse click or mouse over
*/
void DialogueManager::UpdateLabelTreeRoot(DialogueNode* pNode)
{
	pNode->m_pLabel->OnUpdate();
}

/////////////////////////////////////////////////////////////////////////

//invokes onRender on every label in the tree
void DialogueManager::RenderLabelTree(DialogueNode* pNode)
{
	if(pNode == NULL)
		return;
	
	for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
	{
		pNode->m_vNodes[i]->m_pLabel->OnRender(255,255,255,0);
		RenderLabelTree(pNode->m_vNodes[i]);
	}
}

/////////////////////////////////////////////////////////////////////////

//renders the root of the tree only
void DialogueManager::RenderLabelTreeRoot(DialogueNode* pNode)
{
	pNode->m_pLabel->OnRender(255,255,255,0);
}

/////////////////////////////////////////////////////////////////////////
/*
This is the function responsible for changing the current dialogue with another until the user reaches the end of the dialogue
The function first checks if the dialogue is started. The dialogue starts when the user clicks on the root and his children are shown
Then the current dialogue node is the root, his children are revealed and is executed the else if in this function, which 
checks if the user clicked on any if the shown children.If any of the children is clicked isClickedDialogueNode is changed to true
and the clickedDialogueNode variable is changed to the clicked node. The dialogue changing later continues in labelClicked function 
*/
void DialogueManager::ChangeDialogue(DialogueNode* pNode,DialogueObject& dialogueObject)
{
	if( dialogueObject.m_bIsStarted )
	{
		if( AreChildrenHidden(pNode) && !pNode->m_vNodes.empty() )
		{
			HideAllLabelTree(dialogueObject.m_pTree->m_pRoot);

			for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
			{
				pNode->m_vNodes[i]->m_pLabel->SetVisible(true);
			}
			return;
		}
		else if( !AreChildrenHidden(pNode) && !pNode->m_vNodes.empty() )
		{
			for(unsigned int j=0;j<pNode->m_vNodes.size();j++)
			{
			
				if( pNode->m_vNodes[j]->m_pLabel->IsMouseDown() )
				{
					dialogueObject.m_bIsClickedDialogueNode = true;
					dialogueObject.m_pClickedDialogueNode = pNode->m_vNodes[j];	
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/*
when isClickedDialogueNode is true and the user releases the mouse botton this means that 
dialogue node is selected.
Then if the node got quest the user receives it and the dialogue ends.
Otherwise the current dialogue node is changed to the clicked one.
Both labelClicked() and changeDialogue() functions are invoked in onUpdate() in Game.cpp 
Firstly labelClicked is inactive and after node is clicked labelClicked becomes active
and changeDialogue inactive.This way we can detect single click om dialogue - in changeDialogue we detect if the user 
clicked on node and late in labelClicked we detect if the user released the mouse button. 
Click and release combination of mouse button ensures single click.
*/
void DialogueManager::LabelClicked(DialogueObject& dialogueObject,map<string,QuestObject>& mapActiveQuests,map<string,QuestObject>& mapAvailableQuests)
{
	if( dialogueObject.m_bIsClickedDialogueNode && pDinput->IsMouseButtonUp(0) )
	{
		//if there is quest attached to this node, take the quest and end the conversation
		if( !(dialogueObject.m_pClickedDialogueNode->m_strQuest.empty()) )
		{
			HideAllLabelTree(dialogueObject.m_pTree->m_pRoot);
			if( mapAvailableQuests.find(dialogueObject.m_pClickedDialogueNode->m_strQuest)!= mapAvailableQuests.end() )
			{
				mapActiveQuests[dialogueObject.m_pClickedDialogueNode->m_strQuest] = mapAvailableQuests.find(dialogueObject.m_pClickedDialogueNode->m_strQuest)->second;
				mapAvailableQuests.erase(dialogueObject.m_pClickedDialogueNode->m_strQuest);
			}

			SkinnedModel* pGameObject = m_pGameObjManager->GetSkinnedModelByName(dialogueObject.m_strModel);

			pGameObject->SetHasDialogue(false);

			dialogueObject.m_bIsClickedDialogueNode  = false;
			dialogueObject.m_pCurrentDialogueNode	 = dialogueObject.m_pTree->m_pRoot;
			dialogueObject.m_bIsStarted				 = false;
			dialogueObject.m_bIsEnded				 = true;
			return;
		}
		//if the node we clicked got children save him and continue the dialogue from there
		else if( (dialogueObject.m_pClickedDialogueNode->m_strQuest.empty()) && 
				 !dialogueObject.m_pClickedDialogueNode->m_vNodes.empty() )
		{
			dialogueObject.m_bIsClickedDialogueNode = false;
			dialogueObject.m_pCurrentDialogueNode = dialogueObject.m_pClickedDialogueNode;
		}
		//if we reached the end of the conversation and the node doesnt have quest or anything just end the conversation
		else if( (dialogueObject.m_pClickedDialogueNode->m_strQuest.empty()) && 
				  dialogueObject.m_pClickedDialogueNode->m_vNodes.empty() )
		{
			HideAllLabelTree(dialogueObject.m_pTree->m_pRoot);
			dialogueObject.m_pCurrentDialogueNode	 = dialogueObject.m_pTree->m_pRoot;
			dialogueObject.m_bIsClickedDialogueNode  = false;
			dialogueObject.m_bIsStarted				 = false;	
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/*
checks if the user clicked on root nood and if he did started variable is set to true
this function is invoked in onUpdate in Game.cpp and after the dialogue is started 
this function becomes inactive and the active is changeDialogue only.
*/
void DialogueManager::StartDialogue(DialogueNode* pNode,DialogueObject& dialogueObject)
{
	if( pNode->m_pLabel->IsMouseDown() && AreChildrenHidden(pNode) && 
		!pNode->m_vNodes.empty() && !dialogueObject.m_bIsStarted && !dialogueObject.m_bIsEnded )
	{
		HideRoot(dialogueObject.m_pTree->m_pRoot);
		
		HideAllLabelTree(dialogueObject.m_pTree->m_pRoot);
		for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
		{
			pNode->m_vNodes[i]->m_pLabel->SetVisible(true);
		}
		dialogueObject.m_pCurrentDialogueNode = pNode;
		dialogueObject.m_bIsStarted = true;
		
		//hideRoot(obj.tree->root);
	}

}

/////////////////////////////////////////////////////////////////////////
/*
hides all the nodes in the tree. This way all the nodes on the screen become hidden i.e. not rendered 
and are shown only certain nodes after invoking this function.Used in changeLabel(), labelClicked() and startDialogue()
*/
void DialogueManager::HideAllLabelTree(DialogueNode* pNode)
{
	for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
	{
		pNode->m_vNodes[i]->m_pLabel->SetVisible(false);
		HideAllLabelTree(pNode->m_vNodes[i]);
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::HideRoot(DialogueNode* pNode)
{
	pNode->m_pLabel->SetVisible(false);
}

/////////////////////////////////////////////////////////////////////////

//this function checks if the children of the passed node are hidden.Used in changeLabel(), labelClicked()
bool DialogueManager::AreChildrenHidden(DialogueNode* pNode)
{
	for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
	{
		if( !(pNode->m_vNodes[i]->m_pLabel->IsVisible()) )
		{
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////
