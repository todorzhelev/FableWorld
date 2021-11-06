#pragma once
#include<iostream>
#include<vector>

//struct for quests
class Quest {
public:

	Quest() {
		m_strTitle = "";
		m_strContent = "";
		m_strRequiredObject = "";
		m_bIsCompleted = false;
		m_bIsStarted = false;
	}

	std::string	m_strTitle;
	std::string	m_strContent;
	std::string	m_strRequiredObject;

	bool m_bIsCompleted;
	bool m_bIsStarted;
};

class QuestManager {
public:
	static QuestManager* m_pQuestManager;
	std::vector<Quest*>& GetQuests();
private:
	std::vector<Quest*> m_quests;
};

inline QuestManager* GetQuestManager() {
	if (QuestManager::m_pQuestManager == nullptr) {
		QuestManager::m_pQuestManager = new QuestManager();
		return QuestManager::m_pQuestManager;
	}

	return QuestManager::m_pQuestManager;
}