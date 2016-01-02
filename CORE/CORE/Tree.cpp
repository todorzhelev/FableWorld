#include"Tree.h"
#include "Label.h"
#include "TextManager.h"

/////////////////////////////////////////////////////////////////////////

Tree::Tree()
{
	m_pRoot = NULL;
}


/////////////////////////////////////////////////////////////////////////

//returns the text holded by the passed node
string Tree::GetText(node* pNode)
{
	return pNode->m_strText;
}

/////////////////////////////////////////////////////////////////////////

node* Tree::GetRoot()
{
	return m_pRoot;
}

/////////////////////////////////////////////////////////////////////////

void Tree::SetRoot(node* pNode)
{
	m_pRoot = pNode;
}

/////////////////////////////////////////////////////////////////////////

//inserts single node in the tree
node* Tree::InsertNode(node* pNode,string text,int val,float x,float y,string quest)
{
	if( pNode == NULL )
	{
		pNode			 = new node;
		pNode->m_pLabel  = new Label(D3DXVECTOR2(x,y),text);
		pNode->m_nKey	 = val;
		pNode->m_strText = text;
		pNode->m_pLabel->SetVisible(false);
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

void Tree::InsertNode(node*& currentNode, node*& parentNode, string text, string quest)
{
	if (currentNode == nullptr && parentNode == nullptr)
	{
		currentNode = new node;
		currentNode->m_pLabel = new Label(D3DXVECTOR2(5, 620), text);
		//pParentNode->m_nKey = val;
		currentNode->m_strText = text;
		currentNode->m_pLabel->SetVisible(false);
		currentNode->m_strQuest = quest;
		parentNode = currentNode;
	}
	else
	{
		int y = 580 + parentNode->m_vNodes.size() * 40;

		currentNode = new node;
		currentNode->m_pLabel = new Label(D3DXVECTOR2(5, y), text);
		//pNode->m_nKey = val;
		currentNode->m_strText = text;
		currentNode->m_pLabel->SetVisible(false);
		currentNode->m_strQuest = quest;

		parentNode->m_vNodes.push_back(currentNode);
	}

	
}


/////////////////////////////////////////////////////////////////////////
/*
returns the node that corresponds with the passed key.
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
