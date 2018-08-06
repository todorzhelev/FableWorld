#include "DialogueManager.h"
#include "SkinnedModel.h"
#include "LuaFunctions.h"

DialogueManager* pDialogueManager = nullptr;

//TODO: this code should be completly refactored

/////////////////////////////////////////////////////////////////////////////////////////

void DialogueManager::LoadDialogues(std::string strDialoguesFileName)
{
	tinyxml2::XMLDocument doc;
	//loads the xml file
	tinyxml2::XMLError err = doc.LoadFile(strDialoguesFileName.c_str());

	if( err != tinyxml2::XMLError::XML_SUCCESS )
	{
		MessageBox(0,"Cannot load dialogue file ",0,0);
		printf("Dialogue file %s is probably missing \n", strDialoguesFileName.c_str());
	}

	//gets the root element of the whole xml document- Dialogues
	m_pRoot = doc.FirstChildElement("Dialogue");
	if(m_pRoot)
	{
		//gets the root element of single dialogue
		m_pDialogue = m_pRoot;
		while(m_pDialogue)
		{
			DialogueObject* pDialogue = new DialogueObject;
			pDialogue->m_pTree = new Tree;
			pDialogue->m_strModel = m_pDialogue->Attribute("model");
		
			m_pNode = m_pDialogue->FirstChildElement("Node");
		
			TraverseNodes(m_pNode, pDialogue->m_pTree, nullptr, pDialogue->m_pTree->m_pRoot);

			//when the dialogue is rendered and updated it is passed the current node from it.
			//This dialogue isnt started yet so the current node is the root
			pDialogue->m_pCurrentDialogueNode = pDialogue->m_pTree->m_pRoot;

			pDialogue->m_bIsStarted = false;
			pDialogue->m_bIsClickedDialogueNode = false;
			pDialogue->m_bIsEnded = false;

			//adds the loaded from xml dialogue to map
			AddDialogue(pDialogue);

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
	
	std::string quest = "";
	std::string text = "";

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

std::vector<DialogueObject*>& DialogueManager::GetDialogues()
{
	return m_dialogues;
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::OnUpdate()
{
	for (auto& dialogue : m_dialogues)
	{
		if (!dialogue->m_bIsEnded)
		{	
			UpdateDialogue(dialogue->m_pCurrentDialogueNode, *dialogue);
		}

		GameObject* obj = m_pGameObjManager->GetObjectByName(dialogue->m_strModel);

		if (obj->IsPicked() && !dialogue->m_bIsEnded)
		{
			dialogue->m_pTree->m_pRoot->m_pLabel->SetVisible(true);
			obj->SetPicked(false);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::AddDialogue(DialogueObject* dialogueObject)
{
	m_dialogues.push_back(dialogueObject);
}

/////////////////////////////////////////////////////////////////////////

void DialogueManager::UpdateDialogue(DialogueNode* pNode, DialogueObject& dialogue)
{
	if(!dialogue.m_bIsStarted )
	{
		if (pNode->m_pLabel->IsMouseDown())
		{
			for (unsigned int i = 0; i<pNode->m_vNodes.size(); i++)
			{
				pNode->m_vNodes[i]->m_pLabel->SetVisible(true);
			}

			dialogue.m_pCurrentDialogueNode = pNode;
			dialogue.m_bIsStarted = true;
		}
	}
	else
	{
		if (!pNode->m_vNodes.empty())
		{
			//show the children if they are hidden
			if (AreChildrenHidden(pNode))
			{
				HideDialogueTree(dialogue.m_pTree->m_pRoot);

				for (unsigned int i = 0; i<pNode->m_vNodes.size(); i++)
				{
					pNode->m_vNodes[i]->m_pLabel->SetVisible(true);
				}
			}
			//if the children are visible get the clicked child
			else
			{
				for (unsigned int j = 0; j<pNode->m_vNodes.size(); j++)
				{
					if (pNode->m_vNodes[j]->m_pLabel->IsMouseDown())
					{
						dialogue.m_bIsClickedDialogueNode = true;
						dialogue.m_pClickedDialogueNode = pNode->m_vNodes[j];
					}
				}
			}
		}

		//if the mouse button is up we have selected this node
		if (dialogue.m_bIsClickedDialogueNode && pDinput->IsMouseButtonUp(0))
		{
			std::string quest = dialogue.m_pClickedDialogueNode->m_strQuest;

			//if there is quest attached to this node, take the quest and end the conversation
			if (!quest.empty())
			{
				HideDialogueTree(dialogue.m_pTree->m_pRoot);

				//erase the quest from active quests
				auto& quests = GetQuestManager()->GetQuests();

				for (auto it = quests.begin(); it != quests.end(); ++it)
				{
					if (!(*it)->m_strTitle.compare(quest))
					{
						(*it)->m_bIsCompleted = false;
						(*it)->m_bIsStarted = true;
						break;
					}
				}

				SkinnedModel* pGameObject = m_pGameObjManager->GetSkinnedModelByName(dialogue.m_strModel);

				pGameObject->SetHasDialogue(false);

				dialogue.m_pCurrentDialogueNode = dialogue.m_pTree->m_pRoot;
				dialogue.m_bIsStarted = false;
				dialogue.m_bIsEnded = true;
			}
			else
			{
				//if the node we clicked got children save him and continue the dialogue from there
				if (!dialogue.m_pClickedDialogueNode->m_vNodes.empty())
				{
					dialogue.m_pCurrentDialogueNode = dialogue.m_pClickedDialogueNode;
				}
				//if we reached the end of the conversation and the node doesnt have quest or anything just end the conversation
				else
				{
					HideDialogueTree(dialogue.m_pTree->m_pRoot);
					dialogue.m_pCurrentDialogueNode = dialogue.m_pTree->m_pRoot;
					dialogue.m_bIsStarted = false;
				}
			}

			dialogue.m_bIsClickedDialogueNode = false;
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
