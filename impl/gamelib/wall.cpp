#include "wall.hpp"
#include "game_interface.hpp"
#include "game_properties.hpp"
#include "range/v3/view/subrange.hpp"

Wall::Wall(std::shared_ptr<jt::Box2DWorldInterface> world, const b2BodyDef* def)
    : Box2DObject(world, def)
    , m_size { 16, 16 }
{
}

void Wall::doCreate()
{
    m_shape = std::make_shared<jt::Shape>();
    m_shape->makeRect(m_size, getGame()->gfx().textureManager());
    m_shape->setColor(GP::getPalette().getColor(1));
    m_shape->setOffset(m_size * 0.5f);
    m_shape->setOrigin(m_size * 0.5f);

    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 5.0f;
    fixtureDef.restitution = 0.8f;
    b2PolygonShape boxCollider {};
    boxCollider.SetAsBox(8, 8);
    fixtureDef.shape = &boxCollider;
    getB2Body()->CreateFixture(&fixtureDef);
}

void Wall::doUpdate(float const elapsed)
{
    m_shape->setPosition(getPosition());
    m_shape->update(elapsed);
}

void Wall::doDraw() const { m_shape->draw(getGame()->gfx().target()); }
