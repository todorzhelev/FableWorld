#pragma once

#include "../CORE/Recast/Recast/Recast.h"
#include "NavmeshInterfaces.h"
#include "NavmeshPathfinder.h"


/// These are just Navmesh areas to use consistent values across the Navmeshs.
/// The use should specify these base on his needs.
enum NavmeshPolyAreas
{
	NAVMESH_POLYAREA_GROUND,
	NAVMESH_POLYAREA_WATER,
	NAVMESH_POLYAREA_ROAD,
	NAVMESH_POLYAREA_DOOR,
	NAVMESH_POLYAREA_GRASS,
	NAVMESH_POLYAREA_JUMP,
};
enum NavmeshPolyFlags
{
	NAVMESH_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	NAVMESH_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	NAVMESH_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	NAVMESH_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	NAVMESH_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	NAVMESH_POLYFLAGS_ALL = 0xffff	// All abilities.
};

//class NavmeshDebugDraw : public DebugDrawGL
//{
//public:
//	virtual unsigned int areaToCol(unsigned int area);
//};

enum NavmeshPartitionType
{
	NAVMESH_PARTITION_WATERSHED,
	NAVMESH_PARTITION_MONOTONE,
	NAVMESH_PARTITION_LAYERS,
};



class Navmesh
{
protected:

	dtNavMesh* loadAll(const char* path);
	void saveAll(const char* path, const dtNavMesh* mesh);

public:
	Navmesh();
	virtual ~Navmesh();

	void cleanup();

	//void setContext(BuildContext* ctx) { m_ctx = ctx; }

	void setTool(NavmeshTool* tool);
	NavmeshToolState* getToolState(int type) { return m_toolStates[type]; }
	void setToolState(int type, NavmeshToolState* s) { m_toolStates[type] = s; }

	//NavmeshDebugDraw& getDebugDraw() { return m_dd; }

	virtual void handleSettings();
	virtual void handleTools();
	virtual void handleDebugMode();
	virtual void handleClick(const float* s, const float* p, bool shift);
	virtual void handleToggle();
	virtual void handleStep();
	virtual void handleRender();
	virtual void handleRenderOverlay(double* proj, double* model, int* view);
	virtual void handleMeshChanged(class InputGeom* geom);
	virtual bool handleBuild();
	virtual void handleUpdate(const float dt);
	virtual void collectSettings(struct BuildSettings& settings);

	virtual class InputGeom* getInputGeom() { return m_geom; }
	virtual class dtNavMesh* getNavMesh() { return m_navMesh; }
	virtual class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
	virtual class dtCrowd* getCrowd() { return m_crowd; }
	virtual float getAgentRadius() { return m_agentRadius; }
	virtual float getAgentHeight() { return m_agentHeight; }
	virtual float getAgentClimb() { return m_agentMaxClimb; }

	/*unsigned char getNavMeshDrawFlags() const { return m_navMeshDrawFlags; }
	void setNavMeshDrawFlags(unsigned char flags) { m_navMeshDrawFlags = flags; }
*/
	void updateToolStates(const float dt);
	void initToolStates(Navmesh* Navmesh);
	void resetToolStates();
	void renderToolStates();
	void renderOverlayToolStates(double* proj, double* model, int* view);

	void resetCommonSettings();
	void handleCommonSettings();

	void loadGeometry();
	void FindPath(D3DXVECTOR3 startPos, D3DXVECTOR3 endPos);

	std::vector<D3DXVECTOR3> GetCalculatedPath();

private:
	// Explicitly disabled copy constructor and copy assignment operator.
	Navmesh(const Navmesh&);
	Navmesh& operator=(const Navmesh&);

protected:
	InputGeom* m_geom;
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;
	dtCrowd* m_crowd;

	//unsigned char m_navMeshDrawFlags;

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailNavmeshDist;
	float m_detailNavmeshMaxError;
	int m_partitionType;

	bool m_filterLowHangingObstacles;
	bool m_filterLedgeSpans;
	bool m_filterWalkableLowHeightSpans;

	NavmeshTool* m_tool;
	NavmeshToolState* m_toolStates[MAX_TOOLS];

	bool m_keepInterResults;
	float m_totalBuildTimeMs;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh;

	BuildContext* m_ctx;

	//NavmeshDebugDraw m_dd;
};

