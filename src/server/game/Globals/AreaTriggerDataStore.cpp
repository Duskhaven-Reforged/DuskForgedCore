
#include "AreaTriggerDataStore.h"
#include "AreaTriggerTemplate.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Timer.h"
#include "ScriptMgr.h"
#include <cmath>

template <>
struct std::hash<AreaTriggerId>
{
    std::size_t operator()(AreaTriggerId const& value) const noexcept
    {
        size_t hashVal = 0;
        hashVal ^= std::hash<uint32>()(value.Id) + 0x9E3779B9 + (hashVal << 6) + (hashVal >> 2);
        hashVal ^= std::hash<bool>()(value.IsServerSide) + 0x9E3779B9 + (hashVal << 6) + (hashVal >> 2);
        return hashVal;
    }
};

namespace
{
    std::unordered_map<AreaTriggerId, AreaTriggerTemplate> _areaTriggerTemplateStore;
    std::unordered_map<uint32, AreaTriggerCreateProperties> _areaTriggerCreateProperties;

    typedef std::unordered_map<uint32/*cell_id*/, std::set<ObjectGuid::LowType>> AtCellObjectGuidsMap;
}

void AreaTriggerDataStore::LoadAreaTriggerTemplates()
{
    uint32 oldMSTime = getMSTime();
    std::unordered_map<uint32, std::vector<Position>> verticesByCreateProperties;
    std::unordered_map<uint32, std::vector<Position>> verticesTargetByCreateProperties;
    std::unordered_map<uint32, std::vector<Position>> splinesByCreateProperties;
    std::unordered_map<AreaTriggerId, std::vector<AreaTriggerAction>> actionsByAreaTrigger;

    //                                                            0              1             2           3            4
    if (QueryResult templateActions = WorldDatabase.Query("SELECT AreaTriggerId, IsServerSide, ActionType, ActionParam, TargetType FROM `areatrigger_template_actions`"))
    {
        do
        {
            Field* templateActionFields = templateActions->Fetch();
            AreaTriggerId areaTriggerId = { templateActionFields[0].Get<uint32>(), templateActionFields[1].Get<uint8>() == 1 };

            AreaTriggerAction action;
            action.Param = templateActionFields[3].Get<uint32>();
            uint32 actionType = templateActionFields[2].Get<uint32>();
            uint32 targetType = templateActionFields[4].Get<uint32>();

            if (actionType >= AREATRIGGER_ACTION_MAX)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_template_actions` has invalid ActionType ({},{}) for AreaTriggerId {} and Param {}", actionType, areaTriggerId.Id, action.Param);
                continue;
            }

            if (targetType >= AREATRIGGER_ACTION_USER_MAX)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_template_actions` has invalid TargetType ({},{}) for AreaTriggerId {} and Param {}", targetType, areaTriggerId.Id, action.Param);
                continue;
            }

            action.TargetType = AreaTriggerActionUserTypes(targetType);
            action.ActionType = AreaTriggerActionTypes(actionType);

            actionsByAreaTrigger[areaTriggerId].push_back(action);
        } while (templateActions->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates actions. DB table `areatrigger_template_actions` is empty.");
    }

    //                                                     0                              1    2         3         4               5
    if (QueryResult vertices = WorldDatabase.Query("SELECT AreaTriggerCreatePropertiesId, Idx, VerticeX, VerticeY, VerticeTargetX, VerticeTargetY FROM `areatrigger_create_properties_polygon_vertex` ORDER BY `AreaTriggerCreatePropertiesId`, `Idx`"))
    {
        do
        {
            Field* verticeFields = vertices->Fetch();
            uint32 areaTriggerCreatePropertiesId = verticeFields[0].Get<uint32>();

            verticesByCreateProperties[areaTriggerCreatePropertiesId].emplace_back(verticeFields[2].Get<float>(), verticeFields[3].Get<float>());

            if (!verticeFields[4].IsNull() && !verticeFields[5].IsNull())
                verticesTargetByCreateProperties[areaTriggerCreatePropertiesId].emplace_back(verticeFields[4].Get<float>(), verticeFields[5].Get<float>());
            else if (verticeFields[4].IsNull() != verticeFields[5].IsNull())
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties_polygon_vertex` has listed invalid target vertices (AreaTriggerCreatePropertiesId: {}, Index: {}).", areaTriggerCreatePropertiesId, verticeFields[1].Get<uint32>());
        } while (vertices->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger polygon vertices. DB table `areatrigger_create_properties_polygon_vertex` is empty.");
    }

    //                                                    0                              1  2  3
    if (QueryResult splines = WorldDatabase.Query("SELECT AreaTriggerCreatePropertiesId, X, Y, Z FROM `areatrigger_create_properties_spline_point` ORDER BY `AreaTriggerCreatePropertiesId`, `Idx`"))
    {
        do
        {
            Field* splineFields = splines->Fetch();
            uint32 areaTriggerCreatePropertiesId = splineFields[0].Get<uint32>();
            splinesByCreateProperties[areaTriggerCreatePropertiesId].emplace_back(splineFields[1].Get<float>(), splineFields[2].Get<float>(), splineFields[3].Get<float>());
        } while (splines->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger splines. DB table `areatrigger_create_properties_spline_point` is empty.");
    }

    //                                                      0   1             2
    if (QueryResult templates = WorldDatabase.Query("SELECT Id, IsServerSide, Flags FROM `areatrigger_template`"))
    {
        do
        {
            Field* fields = templates->Fetch();

            AreaTriggerTemplate areaTriggerTemplate;
            areaTriggerTemplate.Id.Id = fields[0].Get<uint32>();
            areaTriggerTemplate.Id.IsServerSide = fields[1].Get<uint8>() == 1;
            areaTriggerTemplate.Flags = fields[2].Get<uint32>();

            if (areaTriggerTemplate.Id.IsServerSide && areaTriggerTemplate.Flags != 0)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_template` has listed server-side areatrigger (Id: {}, IsServerSide: {}) with non-zero flags",
                    areaTriggerTemplate.Id.Id, uint32(areaTriggerTemplate.Id.IsServerSide));
                continue;
            }

            areaTriggerTemplate.Actions = std::move(actionsByAreaTrigger[areaTriggerTemplate.Id]);

            _areaTriggerTemplateStore[areaTriggerTemplate.Id] = areaTriggerTemplate;
        } while (templates->NextRow());
    }

    //                                                                        0   1              2            3             4             5              6       7          8                  9             10
    if (QueryResult areatriggerCreateProperties = WorldDatabase.Query("SELECT Id, AreaTriggerId, MoveCurveId, ScaleCurveId, MorphCurveId, FacingCurveId, AnimId, AnimKitId, DecalPropertiesId, TimeToTarget, TimeToTargetScale, "
        //   11     12          13          14          15          16          17          18          19          20
        "Shape, ShapeData0, ShapeData1, ShapeData2, ShapeData3, ShapeData4, ShapeData5, ShapeData6, ShapeData7, ScriptName FROM `areatrigger_create_properties`"))
    {
        do
        {
            AreaTriggerCreateProperties createProperties;

            Field* fields = areatriggerCreateProperties->Fetch();
            createProperties.Id = fields[0].Get<uint32>();

            uint32 areatriggerId = fields[1].Get<uint32>();
            createProperties.Template = GetAreaTriggerTemplate({ areatriggerId, false });

            uint8 shape = fields[11].Get<uint8>();

            if (areatriggerId && !createProperties.Template)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties` references invalid AreaTriggerId {} for AreaTriggerCreatePropertiesId {}", areatriggerId, createProperties.Id);
                continue;
            }

            if (shape >= AREATRIGGER_TYPE_MAX)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties` has listed areatrigger create properties {} with invalid shape {}.",
                    createProperties.Id, uint32(shape));
                continue;
            }

#define VALIDATE_AND_SET_CURVE(Curve, Value) \
            createProperties.Curve = Value; \
            if (createProperties.Curve && !sObjectMgr->GetCurve(createProperties.Curve)) \
            { \
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties` has listed areatrigger (AreaTriggerCreatePropertiesId: {}, Id: {}) with invalid " #Curve " ({}), set to 0!", \
                    createProperties.Id, areatriggerId, createProperties.Curve); \
                createProperties.Curve = 0; \
            }

            VALIDATE_AND_SET_CURVE(MoveCurveId, fields[2].Get<uint32>());
            VALIDATE_AND_SET_CURVE(ScaleCurveId, fields[3].Get<uint32>());
            VALIDATE_AND_SET_CURVE(MorphCurveId, fields[4].Get<uint32>());
            VALIDATE_AND_SET_CURVE(FacingCurveId, fields[5].Get<uint32>());

#undef VALIDATE_AND_SET_CURVE

            createProperties.AnimId = fields[6].Get<int32>();
            createProperties.AnimKitId = fields[7].Get<int32>();

            createProperties.DecalPropertiesId = fields[8].Get<uint32>();

            createProperties.TimeToTarget = fields[9].Get<uint32>();
            createProperties.TimeToTargetScale = fields[10].Get<uint32>();

            createProperties.Shape.Type = static_cast<AreaTriggerTypes>(shape);
            for (uint8 i = 0; i < MAX_AREATRIGGER_ENTITY_DATA; ++i)
                createProperties.Shape.DefaultDatas.Data[i] = fields[12 + i].Get<float>();

            createProperties.scriptName = fields[20].Get<std::string>();
            auto scriptId = 0;
            for (auto& [scriptID, script] : ScriptRegistry<AreaTriggerEntityScript>::ScriptPointerList) {
                if (script->GetName() == createProperties.scriptName)
                    scriptId = scriptID;
            }
            createProperties.ScriptId = scriptId;

            if (shape == AREATRIGGER_TYPE_POLYGON)
            {
                if (createProperties.Shape.PolygonDatas.Height <= 0.0f)
                {
                    createProperties.Shape.PolygonDatas.Height = 1.0f;
                    if (createProperties.Shape.PolygonDatas.HeightTarget <= 0.0f)
                        createProperties.Shape.PolygonDatas.HeightTarget = 1.0f;
                }
            }

            createProperties.PolygonVertices = std::move(verticesByCreateProperties[createProperties.Id]);
            createProperties.PolygonVerticesTarget = std::move(verticesTargetByCreateProperties[createProperties.Id]);
            if (!createProperties.PolygonVerticesTarget.empty() && createProperties.PolygonVertices.size() != createProperties.PolygonVerticesTarget.size())
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties_polygon_vertex` has invalid target vertices, either all or none vertices must have a corresponding target vertex (AreaTriggerCreatePropertiesId: {}).",
                    createProperties.Id);
                createProperties.PolygonVerticesTarget.clear();
            }

            createProperties.SplinePoints = std::move(splinesByCreateProperties[createProperties.Id]);

            _areaTriggerCreateProperties[createProperties.Id] = createProperties;
        } while (areatriggerCreateProperties->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger create properties. DB table `areatrigger_create_properties` is empty.");
    }

    //                                                                  0                              1           2             3                4             5        6                 7
    if (QueryResult circularMovementInfos = WorldDatabase.Query("SELECT AreaTriggerCreatePropertiesId, StartDelay, CircleRadius, BlendFromRadius, InitialAngle, ZOffset, CounterClockwise, CanLoop FROM `areatrigger_create_properties_orbit`"))
    {
        do
        {
            Field* circularMovementInfoFields = circularMovementInfos->Fetch();
            uint32 areaTriggerCreatePropertiesId = circularMovementInfoFields[0].Get<uint32>();

            AreaTriggerCreateProperties* createProperties = Acore::Containers::MapGetValuePtr(_areaTriggerCreateProperties, areaTriggerCreatePropertiesId);
            if (!createProperties)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties_orbit` reference invalid AreaTriggerCreatePropertiesId {}", areaTriggerCreatePropertiesId);
                continue;
            }

            createProperties->OrbitInfo.emplace();

            createProperties->OrbitInfo->StartDelay = circularMovementInfoFields[1].Get<uint32>();

#define VALIDATE_AND_SET_FLOAT(Float, Value) \
            createProperties->OrbitInfo->Float = Value; \
            if (!std::isfinite(createProperties->OrbitInfo->Float)) \
            { \
                LOG_ERROR("sql.sql", "Table `areatrigger_create_properties_orbit` has listed areatrigger (AreaTriggerCreatePropertiesId: {}) with invalid ({}), set to 0!", \
                    areaTriggerCreatePropertiesId, createProperties->OrbitInfo->Float); \
                createProperties->OrbitInfo->Float = 0.0f; \
            }

            VALIDATE_AND_SET_FLOAT(Radius, circularMovementInfoFields[2].Get<float>());
            VALIDATE_AND_SET_FLOAT(BlendFromRadius, circularMovementInfoFields[3].Get<float>());
            VALIDATE_AND_SET_FLOAT(InitialAngle, circularMovementInfoFields[4].Get<float>());
            VALIDATE_AND_SET_FLOAT(ZOffset, circularMovementInfoFields[5].Get<float>());

#undef VALIDATE_AND_SET_FLOAT

            createProperties->OrbitInfo->CounterClockwise = circularMovementInfoFields[6].Get<bool>();
            createProperties->OrbitInfo->CanLoop = circularMovementInfoFields[7].Get<bool>();
        } while (circularMovementInfos->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates circular movement infos. DB table `areatrigger_create_properties_orbit` is empty.");
    }

    LOG_INFO("server.loading", ">> Loaded {} spell areatrigger templates in {} ms.", _areaTriggerTemplateStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

AreaTriggerTemplate const* AreaTriggerDataStore::GetAreaTriggerTemplate(AreaTriggerId const& areaTriggerId) const
{
    return Acore::Containers::MapGetValuePtr(_areaTriggerTemplateStore, areaTriggerId);
}

AreaTriggerCreateProperties* AreaTriggerDataStore::GetAreaTriggerCreateProperties(uint32 areaTriggerCreatePropertiesId) const
{
    return Acore::Containers::MapGetValuePtr(_areaTriggerCreateProperties, areaTriggerCreatePropertiesId);
}

AreaTriggerDataStore* AreaTriggerDataStore::Instance()
{
    static AreaTriggerDataStore instance;
    return &instance;
}
