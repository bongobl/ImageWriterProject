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

Entity::Entity(uint64_t id, EntityData entityData) :
	m_Type((Type)entityData.entityType),
	m_Transform(entityData.transform),
	m_Color(entityData.color),
	m_Id(id),
	m_zOrder(entityData.zOrder)
{
	switch (m_Type) {
		case Type::None:
			std::cerr << "Entity(): Entity type was 'None' " << std::endl;
			exit(1);
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
}

void Entity::applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const
{
	switch (m_Type) {
		case Type::None:
			std::cerr << "Entity::applyCustomDrawingProperties(): Entity type was 'None' " << std::endl;
			exit(1);
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
			switch (params.entityType) {
			case ENTITY_TYPE_NONE:
				std::cerr << "Entity::modify(): Entity type was 'None' " << std::endl;
				exit(1);
			case ENTITY_TYPE_ELLIPSE:
				ellipse = params.ellipse;
				break;
			case ENTITY_TYPE_RECTANGLE:
				rectangle = params.rectangle;
				break;
			case ENTITY_TYPE_TRIANGLE:
				triangle = params.triangle;
				break;
			default:
				std::cerr << "Entity::modify(): Unknown type enum value of " << (unsigned int)m_Type << std::endl;
				exit(1);
		}
		
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_POSITION) {
		bool asOffset = params.transformFieldApplyModes & TRANSFORM_APPLY_MODE_POSITION_AS_OFFSET;
		m_Transform.position = asOffset ? (m_Transform.position + params.transform.position) : params.transform.position;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_ORIENTATION) {
		bool asOffset = params.transformFieldApplyModes & TRANSFORM_APPLY_MODE_ORIENTATION_AS_OFFSET;
		m_Transform.orientation = asOffset ? (m_Transform.orientation + params.transform.orientation) : params.transform.orientation;
	}
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_SCALE) {
		bool asOffset = params.transformFieldApplyModes & TRANSFORM_APPLY_MODE_SCALE_AS_OFFSET;
		m_Transform.scale = asOffset ? (m_Transform.scale * params.transform.scale) : params.transform.scale;
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

EntityData Entity::getData()
{
	switch (m_Type) {
		case Type::None:
			std::cerr << "Entity::getData(): Entity type was 'None' " << std::endl;
			exit(1);
		case Type::Ellipse:
			return {
				.entityType = ENTITY_TYPE_ELLIPSE,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.ellipse = ellipse,
				.entityId = m_Id
			};

		case Type::Rectangle:
			return {
				.entityType = ENTITY_TYPE_RECTANGLE,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.rectangle = rectangle,
				.entityId = m_Id
			};
		case Type::Triangle: {
			return {
				.entityType = ENTITY_TYPE_TRIANGLE,
				.transform = m_Transform,
				.color = m_Color,
				.zOrder = m_zOrder,
				.triangle = triangle,
				.entityId = m_Id
			};
		}
		default:
			std::cerr << "Entity::getData(): Unknown type enum value of " << (unsigned int)m_Type << std::endl;
			exit(1);
	}
}

EntityList::EntityList() :
	m_EllipseVerts(sf::PrimitiveType::TriangleFan, 100),
	m_RectangleVerts(sf::PrimitiveType::TriangleStrip, 4),
	m_TriangleVerts(sf::PrimitiveType::Triangles, 3),
	m_NextEntityId(0x0)
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

EntityList::~EntityList()
{
	removeAllEntities();
}

bool EntityList::addEntity(EntityData params, uint64_t* pNewEntityId, int* pNewZOrder, std::string* pMessage)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	// Note: we can simply pass in pMessage since we're not appending to any 
	// existing locally declared string
	if (!validateZOrder(params.zOrder, m_Entities.size(), pMessage)) {
		return false;
	}

	// Go to insersion spot
	auto iter = m_Entities.begin();
	for (int i = 0; i < params.zOrder; ++i) {
		++iter;
	}

	iter = m_Entities.insert(iter, new Entity(m_NextEntityId, params));

	// Brute force way to align all entity Z-orders to their indices
	realignAllEntityZOrders();

	// Update bookkeeping and output params
	m_EntityIterFromId[m_NextEntityId] = iter;

	if (pNewEntityId) {
		*pNewEntityId = m_NextEntityId;
	}
	if (pNewZOrder) {
		*pNewZOrder = params.zOrder;
	}

	++m_NextEntityId;
	return true;
}

bool EntityList::removeEntity(uint64_t entityId, std::string* pMessage)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	auto mapIter = m_EntityIterFromId.find(entityId);

	// If not found in map
	if (mapIter == m_EntityIterFromId.end()) {
		std::cerr << "Entity id " << entityId << " not found in m_EntityIterFromId map" << std::endl;

		if (pMessage) {
			*pMessage += "No entity with a given ID of " + std::to_string(entityId) + " found";
		}
		return false;
	}

	IterT entityIter = mapIter->second;

	// delete entity
	delete *entityIter;
	*entityIter = nullptr;

	// remove from list and id map
	m_Entities.erase(entityIter);
	m_EntityIterFromId.erase(entityId);

	// Brute force way to align all entity Z-orders to their indices
	realignAllEntityZOrders();

	return true;
}

void EntityList::dummyUpdateShapes(float deltaTime) {
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter) {
		Entity* pCurrEntity = *iter;
		pCurrEntity->m_Transform.orientation += sf::radians(deltaTime).asDegrees();
	}
}

void EntityList::drawShapes(sf::RenderWindow& window, const Camera& camera) {

	std::shared_lock<std::shared_mutex> lock(m_EntitiesMutex);

	m_VertexShader.setUniform("homogFromWorld", camera.getComputedState().glsl_homogFromWorld);

	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter) {
		const Entity* pCurrEntity = *iter;

		m_VertexShader.setUniform("worldFromModel", sf::Glsl::Mat4(sf::fromCore(pCurrEntity->m_Transform)));
		m_VertexShader.setUniform("color", sf::Glsl::Vec4(sf::fromCore(pCurrEntity->m_Color)));

		EntityGeometry* pEntityGeometry = getGeometryForEntity(pCurrEntity->m_Type);
		pCurrEntity->applyCustomDrawingProperties(*pEntityGeometry, m_VertexShader);
		pEntityGeometry->draw(window, m_VertexShader);
	}
}

void EntityList::removeAllEntities() {

	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);
	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter) {
		delete *iter;
		*iter = nullptr;
	}
	m_Entities.clear();
	m_EntityIterFromId.clear();
}

EntityGeometry* EntityList::getGeometryForEntity(Entity::Type type)
{
	switch (type) {
	case Entity::Type::Ellipse:
		return &m_EllipseVerts;
	case Entity::Type::Rectangle:
		return &m_RectangleVerts;
	case Entity::Type::Triangle:
		return &m_TriangleVerts;
	}
	return nullptr;
}

void EntityList::API_addEntity(AddEntityReply* pReply, EntityData params)
{
	std::string message = "";

	pReply->success = addEntity(params, &pReply->entityId, &pReply->zOrder, &message);
	if (message == "") {
		message = "Entity added successfully";
	}
	strcpy(pReply->message, message.c_str());
}

void EntityList::API_getEntityData(EntityDataReply* pReply, uint64_t entityId)
{
	std::shared_lock<std::shared_mutex> lock(m_EntitiesMutex);

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
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

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
	
	IterT entityIter = mapIter->second;
	Entity* pEntity = *entityIter;

	// Validate entity modify params
	pReply->success = pEntity->validateModifyParams(params, &message);
	if (!pReply->success)
	{
		strcpy(pReply->message, message.c_str());
		return;
	}

	// Validate and handle Z order in one shot
	if (params.fieldsToModify & ENTITY_DATA_MODIFIABLE_Z_ORDER && params.zOrder != pEntity->m_zOrder) {

		pReply->success = validateZOrder(params.zOrder, m_Entities.size() - 1 /*size after removal*/, &message);
		if (!pReply->success)
		{
			strcpy(pReply->message, message.c_str());
			return;
		}

		m_Entities.erase(entityIter);

		// Go to insersion spot
		entityIter = m_Entities.begin();
		for (int i = 0; i < params.zOrder; ++i) {
			++entityIter;
		}

		entityIter = m_Entities.insert(entityIter, pEntity);

		// Brute force way to align all entity Z-orders to their indices
		realignAllEntityZOrders();

		// Update bookkeeping
		m_EntityIterFromId[entityId] = entityIter;
	}

	// Modify entity
	pEntity->modify(params);

	if (message == "") {
		message = "Entity " + std::to_string(entityId) + " modified successfully";
	}
	strcpy(pReply->message, message.c_str());
}
void EntityList::API_removeEntity(PlainReply* pReply, uint64_t id)
{
	// tentatively set success string
	std::string message = "";

	pReply->success = removeEntity(id, &message);

	if (message == "") {
		message = "Entity with id " + std::to_string(id) + " removed successfully";
	}
	strcpy(pReply->message, message.c_str());
}

// Private
void EntityList::realignAllEntityZOrders()
{
	int zOrder = 0;
	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter)
	{
		Entity* pCurrEntity = *iter;
		pCurrEntity->m_zOrder = zOrder;
		++zOrder;
	}
}