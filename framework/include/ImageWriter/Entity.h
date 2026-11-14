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

struct EntityGeometry : public sf::VertexArray {
	mutable std::shared_mutex m_Mutex;

	template<typename... Params>
	EntityGeometry(Params... params) : sf::VertexArray(std::forward<Params>(params)...) {};

	void draw(sf::RenderWindow& renderWindow, sf::Shader& vertexShader) const {
		std::shared_lock<std::shared_mutex> lock(m_Mutex);
		renderWindow.draw(*this, &vertexShader);
	};
};

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

	struct DrawUtils {

		EntityGeometry m_EllipseVerts;
		EntityGeometry m_RectangleVerts;
		EntityGeometry m_TriangleVerts;

		sf::Shader m_VertexShader;

		DrawUtils();
		EntityGeometry* getGeometryForEntity(Entity::Type type);
	};

	Type m_Type;
	Transform m_Transform;
	Color m_Color;
	uint64_t m_Id;
	int m_zOrder;

	mutable std::shared_mutex m_Mutex;

	Entity* m_pParent;
	std::list<Entity*> m_Children;

	union {
		EllipseData ellipse;
		RectangleData rectangle;
		TriangleData triangle;
	};
	
	Entity(uint64_t id, EntityData entityData, Entity* pParent);
	Entity(const Entity& other) = delete;
	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const;
	bool validateModifyParams(EntityData params, std::string* pMessage) const;
	void modify(EntityData params);
	
	EntityData getData();

	void draw(sf::RenderWindow& window, DrawUtils& drawUtils, Transform worldFromParent) const;
};

// TODO: rename to scene root
struct EntityList {

	// Managing
	using ListT = std::list<Entity*>;
	using IterT = ListT::iterator;
	uint64_t m_NextEntityId;

	// entity iterators from id map
	std::unordered_map<uint64_t, IterT> m_EntityIterFromId;
	mutable std::shared_mutex m_EntityIterFromIdMutex;

	// Entity root
	std::list<Entity*> m_Entities;
	mutable std::shared_mutex m_EntitiesMutex;

	// Rendering
	Entity::DrawUtils m_DrawUtils;
	
public:

	EntityList();
	~EntityList();

	bool createSpaceForNewEntity(
		std::list<Entity*>& currSiblings, 
		std::shared_mutex& currSiblingsMutex, int& zOrder, IterT& iter,
		std::string* pMessage = nullptr);

	bool removeSpaceForEntity(
		std::list<Entity*>& currSiblings,
		std::shared_mutex& currSiblingsMutex,
		IterT& entityIter,
		std::string* pMessage = nullptr);

	bool changeEntityZOrder(
		std::list<Entity*>& currSiblings,
		std::shared_mutex& currSiblingsMutex,
		IterT& entityIter,
		int& zOrder,
		std::string* pMessage = nullptr);


	void dummyUpdateShapes(float deltaTime);
	void drawShapes(sf::RenderWindow& window, const Camera& camera);
	void removeAllEntities();


	void API_addEntity(AddEntityReply* pReply, EntityData params);
	void API_removeEntity(PlainReply* pReply, uint64_t entityId);
	void API_getEntityData(EntityDataReply* pReply, uint64_t entityId);
	void API_modifyEntity(PlainReply* pReply, EntityData params);
	

private:

	// Brute force Z order setting
	void realignAllEntityZOrders(std::list<Entity*>& currSiblings);
};