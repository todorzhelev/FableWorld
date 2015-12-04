#include "DialogueManager.h"
#include "SkinnedModel.h"

DialogueManager* pDialogueManager = NULL;

/*
SourceFile: Dialogue.cpp
Purpose: This file include the definitions of the functions needed to load dialogue render and update it.
		 The purpose of the dialogue is the player to receive quest depending on his decisions through the dialogue.
		 The dialogue nodes are displayed as text one below another and the user can follow the dialogue by clicking on the desired dialogue
		 The dialogue nodes are organised as a tree of GuiLabels.A class that simulates tree is implemented - in Tree.cpp 
		 and for detecting whether user is clicked on some of the text is used the GuiLabel class.
*/

/////////////////////////////////////////////////////////////////////////
/*
Function: loadDialogues
Purpose: loads all the dialogues from single xml file.
	    The xml file is opened and red element by element. There is one element which is root of the others.
		This element doesnt have parentid attribute.
		After one element is red its value is saved in tree node.
	Example of xml:
	<Dialogues>
		<Dialogue model="?">
			<Leaf id="1" x="5" y="400">Hello,stranger.I need your help.</Leaf>
			<Leaf parentid="1" id="2" x="5" y="420">Ok.I can help.</Leaf> 
			<Leaf parentid="1" id="3" x="5" y="440">No way</Leaf>

			<Leaf parentid="2" id="4" x="5" y="420">Go to spider</Leaf> 
			<Leaf parentid="2" id="5" x="5" y="440">Go to car</Leaf>

			<Leaf parentid="4" id="6" x="5" y="420" quest="Go to the spider">Go to the spider and kill him</Leaf>
		</Dialogue>
	</Dialogues>
	Explanation for reading the file:
	In the code the root variable takes the Dialogues element. 
	dialogue variable takes Dualogue element and leaf variable takes the Leaf element
*/
void DialogueManager::LoadDialogues(string strDialoguesFileName)
{
	TiXmlDocument doc(strDialoguesFileName.c_str());
	//loads the xml file
	if(!doc.LoadFile())
	{
		MessageBox(0,"cant load xml file",0,0);
	}
	//gets the root element of the whole xml document- Dialogues
	m_pRoot = doc.FirstChildElement("Dialogues");
	if(m_pRoot)
	{
		//gets the root element of single dialogue
		m_pDialogue = m_pRoot->FirstChildElement("Dialogue");
		while(m_pDialogue)
		{
				DialogueObject obj;
				obj.m_pTree = new Tree;
				obj.m_strModel = m_pDialogue->Attribute("model");
				m_vGameObjectsWithDialogues.push_back(obj.m_strModel);
		
				m_pLeaf = m_pDialogue->FirstChildElement("Leaf");
				while(m_pLeaf)
				{
					int parentid;
					int id;
					float x;
					float y;
					string quest = "";
					// if the node doesnt have parentid it is root
					if( !m_pLeaf->Attribute("parentid") )
					{
						m_pLeaf->QueryIntAttribute("id",&id);
						m_pLeaf->QueryFloatAttribute("x",&x);
						m_pLeaf->QueryFloatAttribute("y",&y);
						node* pNode = obj.m_pTree->InsertNode(obj.m_pTree->GetRoot(),m_pLeaf->GetText(),id,x,y,quest);
						obj.m_pTree->SetRoot(pNode);
						obj.m_bIsStarted = false;
						obj.m_bIsClickedDialogueNode = false;
						obj.m_bIsEnded = false;
						
					}
					else
					{
						m_pLeaf->QueryIntAttribute("parentid",&parentid);
						m_pLeaf->QueryIntAttribute("id",&id);
						m_pLeaf->QueryFloatAttribute("x",&x);
						m_pLeaf->QueryFloatAttribute("y",&y);
					
						if(m_pLeaf->Attribute("quest"))
						{
							quest = m_pLeaf->Attribute("quest");
						}
						obj.m_pTree->InsertNode(obj.m_pTree->FindNode(obj.m_pTree->GetRoot(),parentid),m_pLeaf->GetText(),id,x,y,quest);
					}
					//goes to the next leaf
					m_pLeaf = m_pLeaf->NextSiblingElement("Leaf");
				}

				//when the dialogue is rendered and updated it is passed the current node from it.
				//This dialogue isnt started yet so the current node is the root
				obj.m_pCurrentDialogueNode = obj.m_pTree->GetRoot();

				//adds the loaded from xml dialogue to map
				AddDialogueObjects(obj);

				//goes to the next Dialogue element
				m_pDialogue = m_pDialogue->NextSiblingElement("Dialogue");
		}
	}

	
}


/////////////////////////////////////////////////////////////////////////
/*
Function: addDialogueObjects
Purpose: add the passed dialogue object to modelDialogue map.
		This map connects actual model in game(dwarf,robot, etc) with dialogue 
		so when the user clicks on the model the dialogue is displayed
*/
void DialogueManager::AddDialogueObjects(DialogueObject& dialogueObject)
{
	m_mapModelDialogue[dialogueObject.m_strModel] = dialogueObject; 
}


/////////////////////////////////////////////////////////////////////////
/*
Function: updateLabelTree
Purpose: invokes the onUpdate function for every label in the tree
		This is needed to check for mouse click or mouse over
*/
void DialogueManager::UpdateLabelTree(node* pNode)
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
Function: updateLabelTree
Purpose: invokes the onUpdate function for the root element in the tree
		This is needed to check for mouse click or mouse over
*/
void DialogueManager::UpdateLabelTreeRoot(node* pNode)
{
	pNode->m_pLabel->OnUpdate();
}


/////////////////////////////////////////////////////////////////////////
/*
Function: renderLabelTree
Purpose: invokes onRender on every label in the tree
*/
void DialogueManager::RenderLabelTree(node* pNode)
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
/*
Function: renderLabelTreeRoot
Purpose: renders the root of the tree only
*/
void DialogueManager::RenderLabelTreeRoot(node* pNode)
{
	pNode->m_pLabel->OnRender(255,255,255,0);
}


/////////////////////////////////////////////////////////////////////////
/*
Function: changeDialogue
Purpose: This is the function responsible for changing the current dialogue with another until the user reaches the end of the dialogue
		The function first checks if the dialogue is started. The dialogue starts when the user clicks on the root and his children are shown
		Then the current dialogue node is the root, his children are revealed and is executed the else if in this function, which 
		checks if the user clicked on any if the shown children.If any of the children is clicked isClickedDialogueNode is changed to true
		and the clickedDialogueNode variable is changed to the clicked node. The dialogue changing later continues in labelClicked function 

*/
void DialogueManager::ChangeDialogue(node* pNode,DialogueObject& dialogueObject)
{
	if( dialogueObject.m_bIsStarted )
	{
		if( AreChildrenHidden(pNode) && !pNode->m_vNodes.empty() )
		{
			HideAllLabelTree(dialogueObject.m_pTree->GetRoot());

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
Function: labelClicked
Purpose: when isClickedDialogueNode is true and the user releases the mouse botton this means that 
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
			HideAllLabelTree(dialogueObject.m_pTree->GetRoot());
			if( mapAvailableQuests.find(dialogueObject.m_pClickedDialogueNode->m_strQuest)!= mapAvailableQuests.end() )
			{
				mapActiveQuests[dialogueObject.m_pClickedDialogueNode->m_strQuest] = mapAvailableQuests.find(dialogueObject.m_pClickedDialogueNode->m_strQuest)->second;
				mapAvailableQuests.erase(dialogueObject.m_pClickedDialogueNode->m_strQuest);
			}

			SkinnedModel* pGameObject = m_pGameObjManager->GetSkinnedModelByName(dialogueObject.m_strModel);

			pGameObject->SetHasDialogue(false);

			dialogueObject.m_bIsClickedDialogueNode  = false;
			dialogueObject.m_pCurrentDialogueNode	 = dialogueObject.m_pTree->GetRoot();
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
			HideAllLabelTree(dialogueObject.m_pTree->GetRoot());
			dialogueObject.m_pCurrentDialogueNode	 = dialogueObject.m_pTree->GetRoot();
			dialogueObject.m_bIsClickedDialogueNode  = false;
			dialogueObject.m_bIsStarted				 = false;	
		}
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function: startDialogue
Purpose: checks if the user clicked on root nood and if he did started variable is set to true
		this function is invoked in onUpdate in Game.cpp and after the dialogue is started 
		this function becomes inactive and the active is changeDialogue only.
*/
void DialogueManager::StartDialogue(node* pNode,DialogueObject& dialogueObject)
{
	if( pNode->m_pLabel->IsMouseDown() && AreChildrenHidden(pNode) && 
		!pNode->m_vNodes.empty() && !dialogueObject.m_bIsStarted && !dialogueObject.m_bIsEnded )
	{
		HideRoot(dialogueObject.m_pTree->GetRoot());
		
		HideAllLabelTree(dialogueObject.m_pTree->GetRoot());
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
Function: hideAllLabelTree
Purpose: hides all the nodes in the tree. This way all the nodes on the screen become hidden i.e. not rendered 
		and are shown only certain nodes after invoking this function.Used in changeLabel(), labelClicked() and startDialogue()
*/
void DialogueManager::HideAllLabelTree(node* pNode)
{
	for(unsigned int i=0;i<pNode->m_vNodes.size();i++)
	{
		pNode->m_vNodes[i]->m_pLabel->SetVisible(false);
		HideAllLabelTree(pNode->m_vNodes[i]);
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function: hideRoot
Purpose: hides the root
*/
void DialogueManager::HideRoot(node* pNode)
{
	pNode->m_pLabel->SetVisible(false);
}


/////////////////////////////////////////////////////////////////////////
/*
Function: areChildrenHidden
Purpose: this function checks if the children of the passed node are hidden.Used in changeLabel(), labelClicked()
*/
bool DialogueManager::AreChildrenHidden(node* pNode)
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
