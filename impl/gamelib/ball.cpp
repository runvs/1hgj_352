#include "ball.hpp"
#include "game_interface.hpp"

Ball::Ball(std::shared_ptr<jt::Box2DWorldInterface> world, b2BodyDef const* def)
    : jt::Box2DObject { world, def }
{
}

void Ball::doCreate()
{
    m_shape = std::make_shared<jt::Shape>();
    m_shape->makeCircle(4, getGame()->gfx().textureManager());
    m_shape->setOffset({ 4.0f, 4.0f });
    m_shape->setOrigin({ 4.0f, 4.0f });

    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 5.0f;
    b2CircleShape circleCollider {};
    circleCollider.m_radius = 4.0f;
    fixtureDef.shape = &circleCollider;
    getB2Body()->CreateFixture(&fixtureDef);
}
void Ball::doUpdate(float const elapsed)
{
    m_shape->setPosition(getPosition());
    m_shape->update(elapsed);
}
void Ball::doDraw() const { m_shape->draw(getGame()->gfx().target()); }
