#include <stdafx.h>
#include "Tree.h"
#include "Label.h"
#include "TextManager.h"
#include "DialogueManager.h"

////////////////////////////////////////////////////////////////////////////

Tree::Tree()
{
	m_pRoot = NULL;
}

////////////////////////////////////////////////////////////////////////////

//returns the text holded by the passed node
std::string Tree::GetText(DialogueNode* pNode)
{
	return pNode->m_strText;
}

////////////////////////////////////////////////////////////////////////////

void Tree::InsertNode(DialogueNode*& currentNode, DialogueNode*& parentNode, std::string text, std::string quest, bool bAnySiblings)
{
	int x = 5;
	int y = 0;
	bAnySiblings ? y = 620 : y = 580;

	if (currentNode == nullptr && parentNode == nullptr)
	{
		currentNode = new DialogueNode;
		currentNode->m_pLabel = new Label(D3DXVECTOR2(x, y+40), text);
		currentNode->m_strText = text;
		currentNode->m_pLabel->SetVisible(false);
		currentNode->m_strQuest = quest;
		parentNode = currentNode;
	}
	else
	{
		y += (parentNode->m_vNodes.size()+1) * 40;

		currentNode = new DialogueNode;
		currentNode->m_pLabel = new Label(D3DXVECTOR2(x, y), text);
		currentNode->m_strText = text;
		currentNode->m_pLabel->SetVisible(false);
		currentNode->m_strQuest = quest;

		parentNode->m_vNodes.push_back(currentNode);
	}
}

/////////////////////////////////////////////////////////////////////////
