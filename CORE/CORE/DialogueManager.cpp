#include "DialogueManager.h"
#include "SkinnedModel.h"
#include "LuaFunctions.h"

DialogueManager* pDialogueManager = nullptr;

//TODO: this code should be completly refactored

/////////////////////////////////////////////////////////////////////////////////////////

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
		
			TraverseNodes(m_pNode, obj.m_pTree, nullptr, obj.m_pTree->m_pRoot);

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

void DialogueManager::TraverseNodes(tinyxml2::XMLElement* xmlNode, Tree* pTree, DialogueNode* parentNode, DialogueNode* currentNode)
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
		TraverseNodes(pFirstChild,pTree, currentNode,nullptr);
	}

	if (pSibling != nullptr)
	{
		TraverseNodes(pSibling,pTree, parentNode,nullptr);
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::OnUpdate(map<string, QuestObject>& activeQuests)
{
	for (auto& dialogue : m_mapModelDialogue)
	{
		if (!dialogue.second.m_bIsEnded)
		{
			pDialogueManager->UpdateDialogueTree(dialogue.second.m_pTree->m_pRoot, true,dialogue.second);
			
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

void DialogueManager::AddDialogueObjects(DialogueObject& dialogueObject)
{
	m_mapModelDialogue[dialogueObject.m_strModel] = dialogueObject; 
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::UpdateDialogueTree(DialogueNode* pNode, bool bIsRootNode, DialogueObject& dialogue)
{
	if(bIsRootNode )
	{
		pNode->m_pLabel->OnUpdate();

		//if the dialogue is not started and we click on the root, start it
		if (pNode->m_pLabel->IsMouseDown() && !dialogue.m_bIsStarted)
		{
			HideDialogueTree(dialogue.m_pTree->m_pRoot);

			for (unsigned int i = 0; i<pNode->m_vNodes.size(); i++)
			{
				pNode->m_vNodes[i]->m_pLabel->SetVisible(true);
			}

			dialogue.m_pCurrentDialogueNode = pNode;
			dialogue.m_bIsStarted = true;
		}

		if (dialogue.m_bIsStarted)
		{
			UpdateDialogueTree(pNode, false, dialogue);
		}
	}
	else
	{
		if (pNode == nullptr)
		{
			return;
		}

		for (unsigned int i = 0; i<pNode->m_vNodes.size(); i++)
		{
			pNode->m_vNodes[i]->m_pLabel->OnUpdate();
			UpdateDialogueTree(pNode->m_vNodes[i],false,dialogue);
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::ChangeDialogue(DialogueNode* pNode, DialogueObject& dialogueObject)
{
	if (dialogueObject.m_bIsStarted && !pNode->m_vNodes.empty())
	{
		if (AreChildrenHidden(pNode))
		{
			HideDialogueTree(dialogueObject.m_pTree->m_pRoot);

			for (unsigned int i = 0; i<pNode->m_vNodes.size(); i++)
			{
				pNode->m_vNodes[i]->m_pLabel->SetVisible(true);
			}
		}
		else
		{
			for (unsigned int j = 0; j<pNode->m_vNodes.size(); j++)
			{
				if (pNode->m_vNodes[j]->m_pLabel->IsMouseDown())
				{
					dialogueObject.m_bIsClickedDialogueNode = true;
					dialogueObject.m_pClickedDialogueNode = pNode->m_vNodes[j];
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::RenderDialogueTree(DialogueNode* pNode)
{
	if (pNode == nullptr)
		return;

	pNode->m_pLabel->OnRender(255, 255, 255, 0);

	for (unsigned int i = 0; i<pNode->m_vNodes.size(); i++)
	{
		RenderDialogueTree(pNode->m_vNodes[i]);
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::LabelClicked(DialogueObject& dialogueObject,map<string,QuestObject>& mapActiveQuests,map<string,QuestObject>& mapAvailableQuests)
{
	if( dialogueObject.m_bIsClickedDialogueNode && pDinput->IsMouseButtonUp(0) )
	{
		//if there is quest attached to this node, take the quest and end the conversation
		if( !(dialogueObject.m_pClickedDialogueNode->m_strQuest.empty()) )
		{
			HideDialogueTree(dialogueObject.m_pTree->m_pRoot);
			if( mapAvailableQuests.find(dialogueObject.m_pClickedDialogueNode->m_strQuest)!= mapAvailableQuests.end() )
			{
				mapActiveQuests[dialogueObject.m_pClickedDialogueNode->m_strQuest] = mapAvailableQuests.find(dialogueObject.m_pClickedDialogueNode->m_strQuest)->second;
				mapAvailableQuests.erase(dialogueObject.m_pClickedDialogueNode->m_strQuest);
			}

			SkinnedModel* pGameObject = m_pGameObjManager->GetSkinnedModelByName(dialogueObject.m_strModel);

			pGameObject->SetHasDialogue(false);

			dialogueObject.m_pCurrentDialogueNode	 = dialogueObject.m_pTree->m_pRoot;
			dialogueObject.m_bIsStarted				 = false;
			dialogueObject.m_bIsEnded				 = true;
		}
		else
		{
			//if the node we clicked got children save him and continue the dialogue from there
			if(!dialogueObject.m_pClickedDialogueNode->m_vNodes.empty())
			{
				dialogueObject.m_pCurrentDialogueNode = dialogueObject.m_pClickedDialogueNode;
			}
			//if we reached the end of the conversation and the node doesnt have quest or anything just end the conversation
			else
			{
				HideDialogueTree(dialogueObject.m_pTree->m_pRoot);
				dialogueObject.m_pCurrentDialogueNode = dialogueObject.m_pTree->m_pRoot;
				dialogueObject.m_bIsStarted = false;
			}
		}

		dialogueObject.m_bIsClickedDialogueNode = false;
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::HideDialogueTree(DialogueNode* pNode)
{
	if( pNode == nullptr )
	{
		return;
	}

	pNode->m_pLabel->SetVisible(false);

	for (auto& node : pNode->m_vNodes)
	{
		HideDialogueTree(node);
	}
}

/////////////////////////////////////////////////////////////////////////

bool DialogueManager::AreChildrenHidden(DialogueNode* pNode)
{
	for(auto& node : pNode->m_vNodes)
	{
		if(node->m_pLabel->IsVisible() )
		{
			return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////
