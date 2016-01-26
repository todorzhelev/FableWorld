#include "QuestManager.h"

QuestManager* QuestManager::m_pQuestManager = nullptr;

std::vector<Quest*>& QuestManager::GetQuests()
{
	return m_quests;
}
