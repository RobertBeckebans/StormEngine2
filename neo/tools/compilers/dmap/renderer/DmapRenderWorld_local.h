/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014-2016 Robert Beckebans
Copyright (C) 2014-2016 Kot in Action Creative Artel

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __DMAPRENDERWORLDLOCAL_H__
#define __DMAPRENDERWORLDLOCAL_H__

// indetifying cvar to determine what virtual screen size to set; must be set when changing aspect ratio
extern idCVar	r_screenAspectRatio;

class idDmapRenderLightLocal;

typedef struct dmapPortal_s
{
	int						intoArea;		// area this portal leads to
	idWinding* 				w;				// winding points have counter clockwise ordering seen this area
	idPlane					plane;			// view must be on the positive side of the plane to cross
	struct dmapPortal_s* 		next;			// next portal of the area
	struct dmapDoublePortal_s* 	doublePortal;
} dmapPortal_t;

typedef struct dmapDoublePortal_s
{
	struct dmapPortal_s*		portals[2];
	int						blockingBits;	// PS_BLOCK_VIEW, PS_BLOCK_AIR, etc, set by doors that shut them off

	// A portal will be considered closed if it is past the
	// fog-out point in a fog volume.  We only support a single
	// fog volume over each portal.
	idDmapRenderLightLocal* 		fogLight;
	struct dmapDoublePortal_s* 	nextFoggedPortal;
} dmapDoublePortal_t;

typedef struct dmapPortalArea_s
{
	int				areaNum;
	int				connectedAreaNum[NUM_PORTAL_ATTRIBUTES];	// if two areas have matching connectedAreaNum, they are
	// not separated by a portal with the apropriate PS_BLOCK_* blockingBits
	int				viewCount;		// set by R_FindViewLightsAndEntities
	dmapPortal_t* 		portals;		// never changes after load
	dmapAreaReference_t	entityRefs;		// head/tail of doubly linked list, may change
	dmapAreaReference_t	lightRefs;		// head/tail of doubly linked list, may change
} dmapPortalArea_t;

class idDmapRenderWorldLocal : public idDmapRenderWorld
{
public:
	idDmapRenderWorldLocal();
	virtual					~idDmapRenderWorldLocal();

	virtual	qhandle_t		AddEntityDef( const dmapRenderEntity_t* re );
	virtual	void			UpdateEntityDef( qhandle_t entityHandle, const dmapRenderEntity_t* re );
	virtual	void			FreeEntityDef( qhandle_t entityHandle );
	virtual const dmapRenderEntity_t* GetRenderEntity( qhandle_t entityHandle ) const;

	virtual	qhandle_t		AddLightDef( const renderLight_t* rlight );
	virtual	void			UpdateLightDef( qhandle_t lightHandle, const renderLight_t* rlight );
	virtual	void			FreeLightDef( qhandle_t lightHandle );
	virtual const renderLight_t* GetRenderLight( qhandle_t lightHandle ) const;

	virtual bool			CheckAreaForPortalSky( int areaNum );

	virtual	void			GenerateAllInteractions();
	virtual void			RegenerateWorld();

	virtual void			ProjectDecalOntoWorld( const idFixedWinding& winding, const idVec3& projectionOrigin, const bool parallel, const float fadeDepth, const idMaterial* material, const int startTime );
	virtual void			ProjectDecal( qhandle_t entityHandle, const idFixedWinding& winding, const idVec3& projectionOrigin, const bool parallel, const float fadeDepth, const idMaterial* material, const int startTime );
	virtual void			ProjectOverlay( qhandle_t entityHandle, const idPlane localTextureAxis[2], const idMaterial* material );
	virtual void			RemoveDecals( qhandle_t entityHandle );

	virtual void			SetRenderView( const dmapRenderView_t* renderView );
	virtual	void			RenderScene( const dmapRenderView_t* renderView );

	virtual	int				NumAreas() const;
	virtual int				PointInArea( const idVec3& point ) const;
	virtual int				BoundsInAreas( const idBounds& bounds, int* areas, int maxAreas ) const;
	virtual	int				NumPortalsInArea( int areaNum );
	virtual exitPortal_t	GetPortal( int areaNum, int portalNum );

	virtual	guiPoint_t		GuiTrace( qhandle_t entityHandle, const idVec3 start, const idVec3 end ) const;
	virtual bool			ModelTrace( dmapModelTrace_t& trace, qhandle_t entityHandle, const idVec3& start, const idVec3& end, const float radius ) const;
	virtual bool			Trace( dmapModelTrace_t& trace, const idVec3& start, const idVec3& end, const float radius, bool skipDynamic = true, bool skipPlayer = false ) const;
	virtual bool			FastWorldTrace( dmapModelTrace_t& trace, const idVec3& start, const idVec3& end ) const;

	virtual void			DebugClearLines( int time );
	virtual void			DebugLine( const idVec4& color, const idVec3& start, const idVec3& end, const int lifetime = 0, const bool depthTest = false );
	virtual void			DebugArrow( const idVec4& color, const idVec3& start, const idVec3& end, int size, const int lifetime = 0 );
	virtual void			DebugWinding( const idVec4& color, const idWinding& w, const idVec3& origin, const idMat3& axis, const int lifetime = 0, const bool depthTest = false );
	virtual void			DebugCircle( const idVec4& color, const idVec3& origin, const idVec3& dir, const float radius, const int numSteps, const int lifetime = 0, const bool depthTest = false );
	virtual void			DebugSphere( const idVec4& color, const idSphere& sphere, const int lifetime = 0, bool depthTest = false );
	virtual void			DebugBounds( const idVec4& color, const idBounds& bounds, const idVec3& org = vec3_origin, const int lifetime = 0 );
	virtual void			DebugBox( const idVec4& color, const idBox& box, const int lifetime = 0 );
	virtual void			DebugFrustum( const idVec4& color, const idFrustum& frustum, const bool showFromOrigin = false, const int lifetime = 0 );
	virtual void			DebugCone( const idVec4& color, const idVec3& apex, const idVec3& dir, float radius1, float radius2, const int lifetime = 0 );
	virtual void			DebugScreenRect( const idVec4& color, const idScreenRect& rect, const dmapViewDef_t* viewDef, const int lifetime = 0 );
	virtual void			DebugAxis( const idVec3& origin, const idMat3& axis );

	virtual void			DebugClearPolygons( int time );
	virtual void			DebugPolygon( const idVec4& color, const idWinding& winding, const int lifeTime = 0, const bool depthTest = false );

	virtual void			DrawText( const char* text, const idVec3& origin, float scale, const idVec4& color, const idMat3& viewAxis, const int align = 1, const int lifetime = 0, bool depthTest = false );


	virtual void 			ClearQuads( int time );			// ########################### SR
	virtual void 			DrawQuad( const idVec4& color, const idWinding& winding, float lifeTime, float fadeTime );	// ########################### SR


	//-----------------------

	idStr					mapName;				// ie: maps/tim_dm2.proc, written to demoFile
	ID_TIME_T					mapTimeStamp;			// for fast reloads of the same level

	areaNode_t* 			areaNodes;
	int						numAreaNodes;

	dmapPortalArea_t* 			portalAreas;
	int						numPortalAreas;
	int						connectedAreaNum;		// incremented every time a door portal state changes

	idScreenRect* 			areaScreenRect;

	dmapDoublePortal_t* 	doublePortals;
	int						numInterAreaPortals;

	idList<idDmapRenderModel*>	localModels;

	idList<idDmapRenderEntityLocal*>	entityDefs;
	idList<idDmapRenderLightLocal*>		lightDefs;

	idBlockAlloc<dmapAreaReference_t, 1024> areaReferenceAllocator;
	idBlockAlloc<idInteraction, 256>	interactionAllocator;
	idBlockAlloc<areaNumRef_t, 1024>	areaNumRefAllocator;

	// all light / entity interactions are referenced here for fast lookup without
	// having to crawl the doubly linked lists.  EnntityDefs are sequential for better
	// cache access, because the table is accessed by light in idDmapRenderWorldLocal::CreateLightDefInteractions()
	// Growing this table is time consuming, so we add a pad value to the number
	// of entityDefs and lightDefs
	idInteraction** 		interactionTable;
	int						interactionTableWidth;		// entityDefs
	int						interactionTableHeight;		// lightDefs


	bool					generateAllInteractionsCalled;

	//-----------------------
	// RenderWorld_load.cpp

	idDmapRenderModel* 			ParseModel( idLexer* src );
	idDmapRenderModel* 			ParseShadowModel( idLexer* src );
	void					SetupAreaRefs();
	void					ParseInterAreaPortals( idLexer* src );
	void					ParseNodes( idLexer* src );
	int						CommonChildrenArea_r( areaNode_t* node );
	void					FreeWorld();
	void					ClearWorld();
	void					FreeDefs();
	void					TouchWorldModels();
	void					AddWorldModelEntities();
	void					ClearPortalStates();
	virtual	bool			InitFromMap( const char* mapName );

	//--------------------------
	// RenderWorld_portals.cpp

	idScreenRect			ScreenRectFromWinding( const idWinding* w, dmapViewEntity_t* space );
	bool					PortalIsFoggedOut( const dmapPortal_t* p );
	void					FloodViewThroughArea_r( const idVec3 origin, int areaNum, const struct portalStack_s* ps );
	void					FlowViewThroughPortals( const idVec3 origin, int numPlanes, const idPlane* planes );
	void					FloodLightThroughArea_r( idDmapRenderLightLocal* light, int areaNum, const struct portalStack_s* ps );
	void					FlowLightThroughPortals( idDmapRenderLightLocal* light );
	areaNumRef_t* 			FloodFrustumAreas_r( const idFrustum& frustum, const int areaNum, const idBounds& bounds, areaNumRef_t* areas );
	areaNumRef_t* 			FloodFrustumAreas( const idFrustum& frustum, areaNumRef_t* areas );
	bool					CullEntityByPortals( const idDmapRenderEntityLocal* entity, const struct portalStack_s* ps );
	void					AddAreaEntityRefs( int areaNum, const struct portalStack_s* ps );
	bool					CullLightByPortals( const idDmapRenderLightLocal* light, const struct portalStack_s* ps );
	void					AddAreaLightRefs( int areaNum, const struct portalStack_s* ps );
	void					AddAreaRefs( int areaNum, const struct portalStack_s* ps );
	void					BuildConnectedAreas_r( int areaNum );
	void					BuildConnectedAreas();
	void					FindViewLightsAndEntities();

	int						NumPortals() const;
	qhandle_t				FindPortal( const idBounds& b ) const;
	void					SetPortalState( qhandle_t portal, int blockingBits );
	int						GetPortalState( qhandle_t portal );
	bool					AreasAreConnected( int areaNum1, int areaNum2, portalConnection_t connection );
	void					FloodConnectedAreas( dmapPortalArea_t* area, int portalAttributeIndex );
	idScreenRect& 			GetAreaScreenRect( int areaNum ) const
	{
		return areaScreenRect[areaNum];
	}
	void					ShowPortals();

	//--------------------------
	// RenderWorld_demo.cpp

	void					StartWritingDemo( idDemoFile* demo );
	void					StopWritingDemo();
	bool					ProcessDemoCommand( idDemoFile* readDemo, dmapRenderView_t* demoRenderView, int* demoTimeOffset );

	void					WriteLoadMap();
	void					WriteRenderView( const dmapRenderView_t* renderView );
	void					WriteVisibleDefs( const dmapViewDef_t* viewDef );
	void					WriteFreeLight( qhandle_t handle );
	void					WriteFreeEntity( qhandle_t handle );
	void					WriteRenderLight( qhandle_t handle, const renderLight_t* light );
	void					WriteRenderEntity( qhandle_t handle, const dmapRenderEntity_t* ent );
	void					ReadRenderEntity();
	void					ReadRenderLight();


	//--------------------------
	// RenderWorld.cpp

	void					ResizeInteractionTable();

	void					AddEntityRefToArea( idDmapRenderEntityLocal* def, dmapPortalArea_t* area );
	void					AddLightRefToArea( idDmapRenderLightLocal* light, dmapPortalArea_t* area );

	void					RecurseProcBSP_r( dmapModelTrace_t* results, int parentNodeNum, int nodeNum, float p1f, float p2f, const idVec3& p1, const idVec3& p2 ) const;

	void					BoundsInAreas_r( int nodeNum, const idBounds& bounds, int* areas, int* numAreas, int maxAreas ) const;

	float					DrawTextLength( const char* text, float scale, int len = 0 );

	void					FreeInteractions();

	void					PushVolumeIntoTree_r( idDmapRenderEntityLocal* def, idDmapRenderLightLocal* light, const idSphere* sphere, int numPoints, const idVec3( *points ), int nodeNum );

	void					PushVolumeIntoTree( idDmapRenderEntityLocal* def, idDmapRenderLightLocal* light, int numPoints, const idVec3( *points ) );

	//-------------------------------
	// tr_light.c
	void					CreateLightDefInteractions( idDmapRenderLightLocal* ldef );
};

#endif /* !__DMAPRENDERWORLDLOCAL_H__ */
