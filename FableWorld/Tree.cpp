#include"Tree.h"
#include "Label.h"
#include "TextManager.h"

/*
SourceFile: Dialogue.cpp
Purpose: simulates tree structure. The tree starts with root and then the root can have as many as you want children 
		 which can have children too.
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:Tree
Purpose:constructor.
*/
Tree::Tree()
{
	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////
/*
Function: GetText
Purpose: returns the text holded by the passed node
*/
string Tree::GetText(node* pNode)
{
	return pNode->m_strText;
}

/////////////////////////////////////////////////////////////////////////
/*
Function: GetRoot
Purpose: returns the root node
*/
node* Tree::GetRoot()
{
	return m_pRoot;
}

/////////////////////////////////////////////////////////////////////////
/*
Function: SetRoot
Purpose: set the root node
*/
void Tree::SetRoot(node* pNode)
{
	m_pRoot = pNode;
}


/////////////////////////////////////////////////////////////////////////
/*
Function: InsertNode
Purpose: inserts single node in the tree
*/
node* Tree::InsertNode(node* pNode,string text,int val,float x,float y,string quest)
{
	if( pNode == NULL )
	{
		pNode			 = new node;
		pNode->m_pLabel  = new Label;
		pNode->m_nKey	 = val;
		pNode->m_strText = text;
		pNode->m_pLabel->SetVisible(false);
		pNode->m_pLabel->SetText(text);
		pNode->m_pLabel->SetHeight(static_cast<int>(GetStringHeight(text)));
		pNode->m_pLabel->SetWidth(static_cast<int>(GetStringWidth(text)));
		pNode->m_pLabel->SetMouseDown(false);
		pNode->m_pLabel->SetMouseOver(false);
		pNode->m_pLabel->SetPosition(D3DXVECTOR2(x,y));
		pNode->m_strQuest = quest;
	}
	else 
	{	
		//resizes the vector which holdes the children of this node
		pNode->m_vNodes.resize(pNode->m_vNodes.size()+1);
		//puts the new node on the back of the vector
		pNode->m_vNodes.back() = InsertNode(NULL,text,val,x,y,quest);
	}
	return pNode;		
}


/////////////////////////////////////////////////////////////////////////
/*
Function: FindNode
Purpose: returns the node that corresponds with the passed key.
		 This is needed so children nodes can be assigned to their parents.
		 In the xml file for dialogues, nodes contains id and parentid. 
		 The nodes that contain parentid got the corresponding node for this id as parent
		 Example:
			<Leaf parentid="1" id="2" x="5" y="420">Ok.I can help.</Leaf> 
			<Leaf parentid="1" id="3" x="5" y="440">No way</Leaf>

			<Leaf parentid="2" id="4" x="5" y="420">Go to spider</Leaf>
			Here "Go to spider" is children of "Ok.I can help.", 
			because "Go to spider" got as parentid 2 which is the id of "Ok.I can help."
*/
node* Tree::FindNode(node* pNode, int key)
{
	if(pNode == NULL)
	{
		return NULL;
	}

	if(pNode->m_nKey == key)
	{
		return pNode;
	}
	else
	{
		for(unsigned int i = 0;i<pNode->m_vNodes.size();i++)
		{
			node* pFoundNode = FindNode(pNode->m_vNodes[i],key);
			if( pFoundNode != NULL ) 
			{
				return pFoundNode;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////
