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

// Note: Should mirror API level EntityType
enum class Core_Internal_EntityType
{
	None = 0,
	Ellipse = 1,
	Rectangle = 2,
	Triangle = 3
};
struct Entity {

	using Type = Core_Internal_EntityType;

	Type m_Type;
	Transform m_Transform;
	Color m_Color;
	uint64_t m_Id;
	int m_zOrder;

	Entity(Type type, uint64_t id, int zOrder, const Transform& transform, Color& color);
	virtual void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const {};
	virtual EntityData getData() const = 0;
};

struct EllipseEntity : Entity {

	EllipseEntity(uint64_t id, int zOrder, const Transform& transform, Color& color, Vec2 halfExtents);

	Vec2 m_HalfExtents;

	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const override;
	EntityData getData() const override;
};

struct RectangleEntity : Entity {

	RectangleEntity(uint64_t id, int zOrder, const Transform& transform, Color& color, Vec2 halfExtents);

	Vec2 m_HalfExtents;

	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const override;
	EntityData getData() const override;
};

struct TriangleEntity : Entity {
	Vec2 m_Point1;
	Vec2 m_Point2;
	Vec2 m_Point3;

	TriangleEntity(uint64_t id, int zOrder, const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3);

	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const override;
	EntityData getData() const override;
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
	void API_removeEntity(PlainReply* pReply, uint64_t entityId);

private:

	// Brute force Z order setting
	void realignAllEntityZOrders();
};