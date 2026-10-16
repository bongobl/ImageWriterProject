#pragma once
#include <ImageWriter/Common.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <shared_mutex>
#include <mutex>

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

	Entity(Type type, const Transform& transform, Color& color);
	virtual void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const {};
};

struct EllipseEntity : Entity {

	EllipseEntity(const Transform& transform, Color& color, Vec2 halfExtents);

	Vec2 m_HalfExtents;

	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const override;
};

struct RectangleEntity : Entity {

	RectangleEntity(const Transform& transform, Color& color, Vec2 halfExtents);

	Vec2 m_HalfExtents;

	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const override;
};

struct TriangleEntity : Entity {
	Vec2 m_Point1;
	Vec2 m_Point2;
	Vec2 m_Point3;

	TriangleEntity(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3);

	void applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const override;
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

	EntityGeometry m_EllipseVerts;
	EntityGeometry m_RectangleVerts;
	EntityGeometry m_TriangleVerts;

	std::vector<Entity*> m_Entities;
	sf::Shader m_VertexShader;
	mutable std::shared_mutex m_EntitiesMutex;
public:

	EntityList();
	~EntityList();

	void addEntity(AddEntityParams params);
	void addEllipse(const Transform& transform, Color& color, Vec2 halfExtents);
	void addRectangle(const Transform& transform, Color& color, Vec2 halfExtents);
	void addTriangle(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3);
	void dummyUpdateShapes(float deltaTime);
	void drawShapes(sf::RenderWindow& window, const Camera& camera);
	void destroyAllShapes();

	EntityGeometry* getGeometryForEntity(Entity::Type type);
};