#include <ImageWriter/Entity.h>
#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <iostream>
#include <cstring>


static bool validateZOrder(int& zOrder, int numEntities, std::string* pMessage)
{
	// Error check on specified Z order
	if (zOrder < -1) {

		if (pMessage) {
			*pMessage += "Error: Invalid Z order specified: " + std::to_string(zOrder) +
				", must specify a value between -1 and the highest available zOrder (" + std::to_string(numEntities) + ")";

		}
		return false;
	}

	// Find true Z order to use
	int correctZOrder = zOrder == -1 ? numEntities : zOrder;
	if (correctZOrder > numEntities) {
		correctZOrder = numEntities;
		if (pMessage) {
			*pMessage += "Warning: Specifying a Z order of " + std::to_string(zOrder) +
				" which is greater than the highest available zOrder (" + std::to_string(numEntities) +
				"), assigning a Z order of " + std::to_string(correctZOrder) + " instead";
		}
	}

	zOrder = correctZOrder;
	return true;
}

Entity::DrawUtils::DrawUtils() :
	m_EllipseVerts(sf::PrimitiveType::TriangleFan, 100),
	m_RectangleVerts(sf::PrimitiveType::TriangleStrip, 4),
	m_TriangleVerts(sf::PrimitiveType::Triangles, 3)
{

	m_RectangleVerts[0].position = sf::Vector2f(1, 1);
	m_RectangleVerts[1].position = sf::Vector2f(-1, 1);
	m_RectangleVerts[2].position = sf::Vector2f(1, -1);
	m_RectangleVerts[3].position = sf::Vector2f(-1, -1);


	constexpr float TWO_PI = 3.14159265358979f * 2;

	int numSlices = m_EllipseVerts.getVertexCount() - 2;
	float sliceSize = (float)TWO_PI / numSlices;

	m_EllipseVerts[0].position = sf::Vector2f(0, 0);
	for (int i = 1; i < m_EllipseVerts.getVertexCount(); ++i) {
		float angle = (i - 1) * sliceSize;
		m_EllipseVerts[i].position = sf::Vector2f(cos(angle), sin(angle));
	}

	if (!m_VertexShader.loadFromFile("entityShader.vert", sf::Shader::Type::Vertex)) {
		std::cerr << "Vertex shader not found" << std::endl;
		exit(1);
	}
}

EntityGeometry* Entity::DrawUtils::getGeometryForEntity(Entity::Type type)
{
	switch (type) {
	case Entity::Type::Undefined:
		std::cerr << "Entity::getGeometryForEntity(): Entity type was 'Undefined' " << std::endl;
		exit(1);
	case Entity::Type::Null:
		return nullptr;
	case Entity::Type::Ellipse:
		return &m_EllipseVerts;
	case Entity::Type::Rectangle:
		return &m_RectangleVerts;
	case Entity::Type::Triangle:
		return &m_TriangleVerts;
	}
	std::cerr << "Entity::getGeometryForEntity(): Unknown type enum value of " << (unsigned int)type << std::endl;
	exit(1);
	return nullptr;
}
Entity::Entity(uint64_t id, EntityData entityData, Entity* pParent) :
	m_Type((Type)entityData.entityType),
	m_Color(entityData.color),
	m_Id(id),
	m_zOrder(entityData.zOrder),
	m_pParent(pParent)
{
	switch (m_Type) {
		case Type::Undefined:
			std::cerr << "Entity(): Entity type was 'Undefined' " << std::endl;
			exit(1);
		case Type::Null:
			break;
		case Type::Ellipse:
			ellipse = entityData.ellipse;
			break;
		case Type::Rectangle:
			rectangle = entityData.rectangle;
			break;
		case Type::Triangle:
			triangle = entityData.triangle;
			break;
		default:
			std::cerr << "Entity(): Unknown type enum value of " << entityData.entityType << std::endl;
			exit(1);
	}

	if (entityData.transformSpace == TRANSFORM_SPACE_LOCAL) {
		m_Transform = entityData.transform;
		syncTransforms(true);
	}
	else if (entityData.transformSpace == TRANSFORM_SPACE_WORLD)
	{
		m_WorldTransform = entityData.transform;
		syncTransforms(false);
	}
	else {
		// the caller should guard against this case
		exit(1);
	}
}

void Entity::applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const
{
	switch (m_Type) {
		case Type::Undefined:
			std::cerr << "Entity::applyCustomDrawingProperties(): Entity type was 'Undefined' " << std::endl;
			exit(1);
		case Type::Null:
			break;
		case Type::Ellipse:
			vertexShader.setUniform("scaleOffset", sf::fromCore(ellipse.halfExtents));
			break;
		case Type::Rectangle:
			vertexShader.setUniform("scaleOffset", sf::fromCore(rectangle.halfExtents));
			break;
		case Type::Triangle: {
			std::unique_lock<std::shared_mutex> lock(geometry.m_Mutex);
			geometry[0].position = sf::fromCore(triangle.point1);
			geometry[1].position = sf::fromCore(triangle.point2);
			geometry[2].position = sf::fromCore(triangle.point3);

			vertexShader.setUniform("scaleOffset", sf::Glsl::Vec2(1, 1));
			break;
		}
		default:
			std::cerr << "Entity::applyCustomDrawingProperties(): Unknown type enum value of " << (unsigned int)m_Type << std::endl;
			exit(1);
	}
}

bool Entity::validateModifyParams(EntityData params, std::string* pMessage) const
{
	Type typeToApply = (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TYPE) ? (Type)params.entityType : m_Type;

	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_HALF_EXTENTS) {

		if (typeToApply != Type::Ellipse && typeToApply != Type::Rectangle) {
			std::cerr << "Entity::modify(): setting 'halfExtents' on type that is not an ellipse or rectangle" << std::endl;
			if (pMessage) {
				*pMessage += "Error: Trying to set 'halfExtents' parameter on a non-ellipse/rectangle entity";
			}
			return false;
		}
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_1) {
		if (typeToApply != Type::Triangle) {
			std::cerr << "Entity::modify(): setting 'point1' parameter on a type that is not a triangle" << std::endl;
			if (pMessage) {
				*pMessage += "Error: Trying to set 'point1' parameter on a non-triangle entity";
			}
			return false;
		}
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_2) {
		if (typeToApply != Type::Triangle) {
			std::cerr << "Entity::modify(): setting 'point2' parameter on a type that is not a triangle" << std::endl;
			if (pMessage) {
				*pMessage += "Error: Trying to set 'point2' parameter on a non-triangle entity";
			}
			return false;
		}
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_3) {
		if (typeToApply != Type::Triangle) {
			std::cerr << "Entity::modify(): setting 'point3' parameter on a type that is not a triangle" << std::endl;
			if (pMessage) {
				*pMessage += "Error: Trying to set 'point3' parameter on a non-triangle entity, ignoring... ";
			}
			return false;
		}
	}
	return true;
}
void Entity::modify(EntityData params)
{
	std::unique_lock<std::shared_mutex> lock(m_Mutex);

	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TYPE) {

		m_Type = (Type)params.entityType;
		switch (m_Type) {
			case Type::Undefined:
				std::cerr << "Entity::modify(): can't modify entity type to be 'Undefined' " << std::endl;
				exit(1);
			case Type::Null:
				break;
			case Type::Ellipse:
				ellipse = params.ellipse;
				break;
			case Type::Rectangle:
				rectangle = params.rectangle;
				break;
			case Type::Triangle:
				triangle = params.triangle;
				break;
			default:
				std::cerr << "Entity::modify(): Unknown type enum value of " << (unsigned int)m_Type << std::endl;
				exit(1);
		}
		
	}

	// transform modifiers
	bool localTransformModified = false;
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_POSITION) {
		bool asOffset = params.transformFieldApplyModes & TRANSFORM_APPLY_MODE_POSITION_AS_OFFSET;
		m_Transform.position = asOffset ? (m_Transform.position + params.transform.position) : params.transform.position;
		localTransformModified = true;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_ORIENTATION) {
		bool asOffset = params.transformFieldApplyModes & TRANSFORM_APPLY_MODE_ORIENTATION_AS_OFFSET;
		m_Transform.orientation = asOffset ? (m_Transform.orientation + params.transform.orientation) : params.transform.orientation;
		localTransformModified = true;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_SCALE) {
		bool asOffset = params.transformFieldApplyModes & TRANSFORM_APPLY_MODE_SCALE_AS_OFFSET;
		m_Transform.scale = asOffset ? (m_Transform.scale * params.transform.scale) : params.transform.scale;
		localTransformModified = true;
	}
	if (localTransformModified) {
		syncTransforms(true);
	}

	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_COLOR) {
		m_Color = params.color;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_Z_ORDER) {
		m_zOrder = params.zOrder;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_HALF_EXTENTS) {
		ellipse.halfExtents = params.ellipse.halfExtents;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_1) {
		triangle.point1 = params.triangle.point1;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_2) {
		triangle.point2 = params.triangle.point2;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_TRIANGLE_POINT_3) {
		triangle.point3 = params.triangle.point3;
	}
}

void Entity::updateDummy(float deltaTime)
{
	std::unique_lock<std::shared_mutex> lock(m_Mutex);

	if (m_Id != ROOT_ENTITY_ID) { // Root entity should not move
		m_Transform.orientation += sf::radians(deltaTime).asDegrees();
		syncTransforms(true);
	}
	
	for (auto iter = m_Children.begin(); iter != m_Children.end(); ++iter) {
		Entity* pCurrEntity = *iter;

		pCurrEntity->updateDummy(deltaTime);
	}
}
EntityData Entity::getData()
{
	uint64_t parentId = m_pParent ? m_pParent->m_Id : ROOT_ENTITY_ID;
	switch (m_Type) {
		case Type::Undefined:
			std::cerr << "Entity::getData(): Entity type was 'None' " << std::endl;
			exit(1);
		case Type::Null:
			return {
				.entityType = ENTITY_TYPE_NULL,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.entityId = m_Id,
				.parentEntityId = parentId
			};
		case Type::Ellipse:
			return {
				.entityType = ENTITY_TYPE_ELLIPSE,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.ellipse = ellipse,
				.entityId = m_Id,
				.parentEntityId = parentId
			};

		case Type::Rectangle:
			return {
				.entityType = ENTITY_TYPE_RECTANGLE,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.rectangle = rectangle,
				.entityId = m_Id,
				.parentEntityId = parentId
			};
		case Type::Triangle: {
			return {
				.entityType = ENTITY_TYPE_TRIANGLE,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.triangle = triangle,
				.entityId = m_Id,
				.parentEntityId = parentId
			};
		}
		default:
			std::cerr << "Entity::getData(): Unknown type enum value of " << (unsigned int)m_Type << std::endl;
			exit(1);
	}
}

void Entity::draw(sf::RenderWindow& window, DrawUtils& drawUtils) const
{
	drawUtils.m_VertexShader.setUniform("worldFromModel", sf::Glsl::Mat4(sf::fromCore(m_WorldTransform)));
	drawUtils.m_VertexShader.setUniform("color", sf::Glsl::Vec4(sf::fromCore(m_Color)));

	// draw current
	EntityGeometry* pEntityGeometry = drawUtils.getGeometryForEntity(m_Type);

	// draw if there is geometry to draw
	if (pEntityGeometry) {
		applyCustomDrawingProperties(*pEntityGeometry, drawUtils.m_VertexShader);
		pEntityGeometry->draw(window, drawUtils.m_VertexShader);
	}
	
	// Note this doesn't respect Z order of parent among siblings
	for (auto iter = m_Children.begin(); iter != m_Children.end(); ++iter) {
		Entity* pCurrChildEntity = *iter;
		pCurrChildEntity->draw(window, drawUtils);
	}
}

// Private helpers
void Entity::syncTransforms(bool syncWorldToLocal)
{
	if (syncWorldToLocal) {
		// if m_Transform was just modified and we need to update m_WorldTransform
		m_WorldTransform = m_pParent ? m_pParent->m_WorldTransform * m_Transform : m_Transform;
	}
	else {
		// if m_WorldTransform was just modified and we need to update m_Transform
		m_Transform = m_pParent ? inverse(m_pParent->m_WorldTransform) * m_WorldTransform : m_WorldTransform;
	}

	// update the world transforms of children
	for (auto iter = m_Children.begin(); iter != m_Children.end(); ++iter) {
		Entity* pCurrEntity = *iter;

		// changing 'this' entity's transform will only ever change its childrens' world transforms
		// so we always sync their world to their local
		pCurrEntity->syncTransforms(true);	
	}
}

EntityList::EntityList() :
	m_NextEntityId(ROOT_ENTITY_ID + 1),
	m_RootEntity(ROOT_ENTITY_ID, { .entityType = ENTITY_TYPE_NULL }, nullptr)
{
	
}

EntityList::~EntityList()
{
	deleteAllEntities();
}

bool EntityList::addChildToParent(
	Entity* pChild, Entity* pParent, int& zOrder, 
	IterT& iter, std::string* pMessage)
{
	std::unique_lock<std::shared_mutex> lock(pParent->m_Mutex);

	// Note: we can simply pass in pMessage since we're not appending to any 
	// existing locally declared string
	if (!validateZOrder(zOrder, pParent->m_Children.size(), pMessage)) {
		return false;
	}

	// Go to insersion spot
	iter = pParent->m_Children.begin();
	for (int i = 0; i < zOrder; ++i) {
		++iter;
	}

	iter = pParent->m_Children.insert(iter, pChild);

	// Brute force way to align all entity Z-orders to their indices
	realignAllEntityZOrders(pParent->m_Children);
	
	return true;
}

bool EntityList::removeEntityFromParent(
	Entity* pParent, IterT entityIter,
	std::string* pMessage)
{
	std::unique_lock<std::shared_mutex> lock(pParent->m_Mutex);

	// remove from list
	pParent->m_Children.erase(entityIter);
	
	// Brute force way to align all entity Z-orders to their indices
	realignAllEntityZOrders(pParent->m_Children);

	return true;
}

bool EntityList::changeEntityZOrderInParent(
	Entity* pParent,
	IterT& entityIter,
	int& zOrder,
	std::string* pMessage)
{

	std::unique_lock<std::shared_mutex> lock(pParent->m_Mutex);

	if (!validateZOrder(zOrder, pParent->m_Children.size() - 1 /*size after removal*/, pMessage)) {
		return false;
	}

	Entity* pEntity = *entityIter;
	pParent->m_Children.erase(entityIter);

	// Go to insersion spot
	entityIter = pParent->m_Children.begin();
	for (int i = 0; i < zOrder; ++i) {
		++entityIter;
	}

	entityIter = pParent->m_Children.insert(entityIter, pEntity);

	// Brute force way to align all entity Z-orders to their indices
	realignAllEntityZOrders(pParent->m_Children);

	return true;
}
void EntityList::dummyUpdateShapes(float deltaTime) {

	m_RootEntity.updateDummy(deltaTime);
}

void EntityList::drawShapes(sf::RenderWindow& window, const Camera& camera) {

	std::shared_lock<std::shared_mutex> lock(m_RootEntity.m_Mutex);

	// homogFromWorld is the same for all entities
	m_DrawUtils.m_VertexShader.setUniform("homogFromWorld", camera.getComputedState().glsl_homogFromWorld);

	for (auto iter = m_RootEntity.m_Children.begin(); iter != m_RootEntity.m_Children.end(); ++iter) {
		const Entity* pCurrEntity = *iter;

		pCurrEntity->draw(window, m_DrawUtils);
		
	}
}

void EntityList::deleteAllEntities() 
{
	std::unique_lock<std::shared_mutex> lock(m_EntityIterFromIdMutex);
	deleteEntity(&m_RootEntity);
	m_EntityIterFromId.clear();
}

void EntityList::API_addEntity(AddEntityReply* pReply, EntityData params)
{

	std::unique_lock<std::shared_mutex> lock(m_EntityIterFromIdMutex);

	std::string message = "";

	Entity* pParentEntity = &m_RootEntity;

	if (params.parentEntityId != ROOT_ENTITY_ID) {

		IterT parentIter;
		if (!getEntityIter(parentIter, params.parentEntityId)) {
			std::cerr << "Parent entity id " << params.parentEntityId << " not found in m_EntityIterFromId map" << std::endl;

			message = "No entity with a given ID of " + std::to_string(params.parentEntityId) + " found";
			strcpy(pReply->message, message.c_str());
			pReply->success = false;
			return;
		}

		pParentEntity = *parentIter;
	}


	IterT listIter;
	Entity* newEntity = new Entity(m_NextEntityId, params, pParentEntity);
	pReply->success = addChildToParent(newEntity, pParentEntity, params.zOrder, listIter, &message);

	if (!pReply->success) {

		delete newEntity;
		strcpy(pReply->message, message.c_str());
		return;
	}

	// Update bookkeeping and output params
	m_EntityIterFromId[m_NextEntityId] = listIter;

	// Add zOrder and entity id to reply
	pReply->entityId = m_NextEntityId;
	pReply->zOrder = params.zOrder;

	++m_NextEntityId;

	if (message == "") {
		message = "Entity added successfully";
	}
	strcpy(pReply->message, message.c_str());
}

void EntityList::API_removeEntity(PlainReply* pReply, uint64_t entityId)
{

	std::unique_lock<std::shared_mutex> lock(m_EntityIterFromIdMutex);

	// tentatively set success string
	std::string message = "";

	IterT entityIter;
	if (!getEntityIter(entityIter, entityId)) {
		std::cerr << "Entity id " << entityId << " not found in m_EntityIterFromId map" << std::endl;

		message = "No entity with a given ID of " + std::to_string(entityId) + " found";
		strcpy(pReply->message, message.c_str());
		pReply->success = false;
		return;
	}

	Entity* pPluckedEntity = *entityIter;

	pReply->success = removeEntityFromParent(pPluckedEntity->m_pParent, entityIter, &message);

	// delete entity and its children
	deleteEntity(pPluckedEntity);

	if (message == "") {
		message = "Entity with id " + std::to_string(entityId) + " removed successfully";
	}
	strcpy(pReply->message, message.c_str());
}

void EntityList::API_getEntityData(EntityDataReply* pReply, uint64_t entityId)
{
	std::shared_lock<std::shared_mutex> mapLock(m_EntityIterFromIdMutex);

	std::string message = "";

	auto mapIter = m_EntityIterFromId.find(entityId);

	// If not found in map
	if (mapIter == m_EntityIterFromId.end()) {
		std::cerr << "Entity id " << entityId << " not found in m_EntityIterFromId map" << std::endl;

		message += "No entity with a given ID of " + std::to_string(entityId) + " found";
		pReply->success = false;
	}
	else {
		IterT entityIter = mapIter->second;
		Entity* pEntity = *entityIter;

		// lock parent's list of children in shared mode
		std::shared_lock<std::shared_mutex> entitiesLock(pEntity->m_pParent->m_Mutex);
		pReply->entityData = pEntity->getData();
		pReply->success = true;
	}

	if (message == "") {
		message = "Entity with id " + std::to_string(entityId) + " found";
	}

	strcpy(pReply->message, message.c_str());
}

void EntityList::API_modifyEntity(PlainReply* pReply, EntityData params)
{

	std::shared_lock<std::shared_mutex> mapLock(m_EntityIterFromIdMutex);

	// tentatively set success string
	std::string message = "";

	uint64_t entityId = params.entityId;

	auto mapIter = m_EntityIterFromId.find(entityId);

	// If not found in map
	if (mapIter == m_EntityIterFromId.end()) {
		std::cerr << "Entity id " << entityId << " not found in m_EntityIterFromId map" << std::endl;

		message = "No entity with a given ID of " + std::to_string(entityId) + " found";
		strcpy(pReply->message, message.c_str());
		pReply->success = false;
		return;
	}
	
	IterT& currLocation = mapIter->second;
	Entity* pEntity = *currLocation;

	// Validate entity modify params
	pReply->success = pEntity->validateModifyParams(params, &message);
	if (!pReply->success)
	{
		strcpy(pReply->message, message.c_str());
		return;
	}

	// Validate and handle Z order in one shot
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_Z_ORDER && params.zOrder != pEntity->m_zOrder) {

		pReply->success = changeEntityZOrderInParent(pEntity->m_pParent, currLocation, params.zOrder, &message);

		if (!pReply->success)
		{
			strcpy(pReply->message, message.c_str());
			return;
		}
	}

	// Modify entity
	pEntity->modify(params);

	if (message == "") {
		message = "Entity " + std::to_string(entityId) + " modified successfully";
	}
	strcpy(pReply->message, message.c_str());
}

bool EntityList::getEntityIter(IterT& entityIter, uint64_t entityId)
{
	auto mapIter = m_EntityIterFromId.find(entityId);

	if (mapIter != m_EntityIterFromId.end()) {
		entityIter = mapIter->second;
		return true;
	}
	return false;
}

void EntityList::deleteEntity(Entity* pEntity)
{
	std::unique_lock<std::shared_mutex> lock(pEntity->m_Mutex);

	// remove and delete all children first
	for (auto iter = pEntity->m_Children.begin(); iter != pEntity->m_Children.end(); ++iter) {

		Entity* pCurrChild = *iter;
		deleteEntity(pCurrChild);
		*iter = nullptr;

	}
	pEntity->m_Children.clear();

	if (pEntity->m_Id != ROOT_ENTITY_ID) {
		m_EntityIterFromId.erase(pEntity->m_Id);
		delete pEntity;
		pEntity = nullptr;
	}
}

// Private
void EntityList::realignAllEntityZOrders(std::list<Entity*>& currSiblings)
{
	int zOrder = 0;
	for (auto iter = currSiblings.begin(); iter != currSiblings.end(); ++iter)
	{
		Entity* pCurrEntity = *iter;
		if (pCurrEntity) {
			pCurrEntity->m_zOrder = zOrder;
			++zOrder;
		}
	}
}