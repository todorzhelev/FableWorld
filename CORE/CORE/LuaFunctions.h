#include "Misc.h"
#include "StaticModel.h"
#include "SkinnedModel.h"
#include "DialogueManager.h"
#include "IBaseScene.h"
#include <string.h>
#include "GameObjectManager.h"
#include "QuestManager.h"

extern std::string mainHero;

int l_addStaticModel(lua_State* L);
int l_addAnimatedModel(lua_State* L);
int l_addQuest(lua_State* L);
int l_setUpMainHero(lua_State* L);