#pragma once
#include <ImageWriter/Common.h>
#include <SFML/Graphics.hpp>
#include <list>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <cstdint>
class Camera;
class EntityGeometry;

// Note: Must mirror API level EntityType
enum class Core_Internal_EntityType
{
	None = ENTITY_TYPE_NONE,
	Ellipse = ENTITY_TYPE_ELLIPSE,
	Rectangle = ENTITY_TYPE_RECTANGLE,
	Triangle = ENTITY_TYPE_TRIANGLE
};
struct Entity {

	using Type = Core_Internal_EntityType;

	Type m_Type;
	Transform m_Transform;
	Color m_Color;
	uint64_t m_Id;
	int m_zOrder;

	mutable std::shared_mutex m_Mutex;

	union {
		EllipseData ellipse;
		RectangleData rectangle;
		TriangleData triangle;
	};
	
	Entity(uint64_t id, EntityData entityData);
	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const;
	bool modify(EntityData params, std::string* pMessage = nullptr);
	EntityData getData();
};

struct EntityGeometry : public sf::VertexArray {
	mutable std::shared_mutex m_Mutex;

	template<typename... Params>
	EntityGeometry(Params... params) : sf::VertexArray(std::forward<Params>(params)...) {};

	void draw(sf::RenderWindow& renderWindow, sf::Shader& vertexShader) const {
		std::shared_lock<std::shared_mutex> lock(m_Mutex);
		renderWindow.draw(*this, &vertexShader);
	};
};
struct EntityList {

	// Managing
	using ListT = std::list<Entity*>;
	using IterT = ListT::iterator;
	uint64_t m_NextEntityId;
	std::unordered_map<uint64_t, IterT> m_EntityIterFromId;
	std::list<Entity*> m_Entities;

	// Rendering
	EntityGeometry m_EllipseVerts;
	EntityGeometry m_RectangleVerts;
	EntityGeometry m_TriangleVerts;
	sf::Shader m_VertexShader;
	
	mutable std::shared_mutex m_EntitiesMutex;
public:

	EntityList();
	~EntityList();

	bool addEntity(EntityData params, uint64_t* pNewEntityId = nullptr, int* pNewZOrder = nullptr, std::string* pMessage = nullptr);
	bool removeEntity(uint64_t id, std::string* pMessage = nullptr);
	void dummyUpdateShapes(float deltaTime);
	void drawShapes(sf::RenderWindow& window, const Camera& camera);
	void removeAllEntities();

	EntityGeometry* getGeometryForEntity(Entity::Type type);

	void API_addEntity(AddEntityReply* pReply, EntityData params);
	void API_getEntityData(EntityDataReply* pReply, uint64_t entityId);
	void API_modifyEntity(PlainReply* pReply, uint64_t entityId, EntityData params);
	void API_removeEntity(PlainReply* pReply, uint64_t entityId);

private:

	// Brute force Z order setting
	void realignAllEntityZOrders();
};