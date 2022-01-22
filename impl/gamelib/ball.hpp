#ifndef INC_1HGJ352_BALL_HPP
#define INC_1HGJ352_BALL_HPP

#include "box2dwrapper/box2d_object.hpp"
#include "shape.hpp"

class Ball : public jt::Box2DObject {
public:
    Ball(std::shared_ptr<jt::Box2DWorldInterface> world, const b2BodyDef* def);
    std::shared_ptr<jt::Shape> m_shape;

    void doCreate() override;
    void doUpdate(float const /*elapsed*/) override;
    void doDraw() const override;

private:
};

#endif // INC_1HGJ352_BALL_HPP
