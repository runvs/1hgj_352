#include "state_game.hpp"
#include "box2dwrapper/box2d_world_impl.hpp"
#include "collision.hpp"
#include "color.hpp"
#include "game_interface.hpp"
#include "game_properties.hpp"
#include "hud/hud.hpp"
#include "random/random.hpp"
#include "shape.hpp"
#include "sprite.hpp"
#include "state_menu.hpp"
#include "tweens/tween_alpha.hpp"
#include "wall.hpp"

void StateGame::doInternalCreate()
{
    m_world = std::make_shared<jt::Box2DWorldImpl>(jt::Vector2f { 0.0f, 100.0f });

    float const w = static_cast<float>(GP::GetWindowSize().x);
    float const h = static_cast<float>(GP::GetWindowSize().y);

    using jt::Shape;
    using jt::TweenAlpha;

    m_background = std::make_shared<Shape>();
    m_background->makeRect({ w, h }, getGame()->gfx().textureManager());
    m_background->setColor(GP::PaletteBackground());
    m_background->setIgnoreCamMovement(true);
    m_background->update(0.0f);

    m_overlay = std::make_shared<Shape>();
    m_overlay->makeRect(jt::Vector2f { w, h }, getGame()->gfx().textureManager());
    m_overlay->setColor(jt::Color { 0, 0, 0 });
    m_overlay->setIgnoreCamMovement(true);
    m_overlay->update(0);
    auto tw = TweenAlpha::create(m_overlay, 0.5f, std::uint8_t { 255 }, std::uint8_t { 0 });
    tw->setSkipFrames();
    tw->addCompleteCallback([this]() { m_running = true; });
    add(tw);

    m_vignette = std::make_shared<jt::Sprite>("#v#"
            + std::to_string(static_cast<int>(GP::GetScreenSize().x)) + "#"
            + std::to_string(static_cast<int>(GP::GetScreenSize().y)),
        getGame()->gfx().textureManager());
    m_vignette->setIgnoreCamMovement(true);
    m_vignette->setColor({ 255, 255, 255, 100 });

    m_hud = std::make_shared<Hud>();
    add(m_hud);

    createBall();

    createWalls();

    m_PaddleLeft = std::make_shared<jt::Shape>();
    m_PaddleLeft->makeRect(jt::Vector2f { 48, 48 }, getGame()->gfx().textureManager());
    m_PaddleLeft->setColor(jt::colors::Transparent);
    m_PaddleLeft->setPosition(jt::Vector2f { 96 - 32, 400 - 48 });

    m_PaddleRight = std::make_shared<jt::Shape>();
    m_PaddleRight->makeRect(jt::Vector2f { 48, 48 }, getGame()->gfx().textureManager());
    m_PaddleRight->setColor(jt::colors::Transparent);
    m_PaddleRight->setPosition(jt::Vector2f { 300 - 96 - 48 + 32, 400 - 48 });
    // StateGame will call drawObjects itself.
    setAutoDraw(false);
}

void StateGame::createWall(jt::Vector2f const& pos)
{
    b2BodyDef groundBodyDef;
    groundBodyDef.fixedRotation = true;
    groundBodyDef.position = jt::Conversion::vec(pos);
    auto w = std::make_shared<Wall>(m_world, &groundBodyDef);
    add(w);
}

void StateGame::createWalls()
{
    b2BodyDef groundBodyDef;
    groundBodyDef.fixedRotation = true;
    for (int i = 0; i != 30; ++i) {
        auto const i_as_float = static_cast<float>(i);
        // ceiling
        createWall(jt::Vector2f { i_as_float * 16.0f, 0.0f });

        if (i < 5 || i > 13) {

            // floor
            createWall(jt::Vector2f { i_as_float * 16.0f, 400.0f - 16.0f });
        }
    }
    for (int i = 0; i != 30; ++i) {
        // walls
        auto const i_as_float = static_cast<float>(i);

        createWall(jt::Vector2f { 0.0f, 16.0f * i_as_float });
        createWall(jt::Vector2f { 300.0f - 16.0f, 16.0f * i_as_float });
    }
}

void StateGame::createBall()
{
    b2BodyDef bodyDef;
    bodyDef.fixedRotation = true;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(100.0f, 100.0f);

    m_ball = std::make_shared<Ball>(m_world, &bodyDef);
    add(m_ball);
}

void StateGame::doInternalUpdate(float const elapsed)
{
    if (m_running) {
        m_world->step(elapsed, GP::PhysicVelocityIterations(), GP::PhysicPositionIterations());
        // update game logic here
        auto const bp = m_ball->getPosition();

        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::A)) {
            m_PaddleLeft->flash(0.4f, GP::getPalette().getColor(4));
            auto const pp = m_PaddleLeft->getPosition();
            if (bp.y > pp.y && bp.y < pp.y + m_PaddleLeft->getGlobalBounds().height) {
                if (bp.x > pp.x && bp.x < pp.x + m_PaddleLeft->getGlobalBounds().width) {
                    auto oldV = m_ball->getVelocity();
                    float vxOffset = jt::Random::getFloatGauss(-10, 5);
                    if (oldV.y > 0) {
                        oldV.y = -oldV.y - jt::Random::getFloatGauss(+10, 5);
                    } else {
                        oldV.y = -200;
                    }
                    m_ball->setVelocity(jt::Vector2f { oldV.x + vxOffset, oldV.y });
                }
            }
        }
        
        if (getGame()->input().keyboard()->justPressed(jt::KeyCode::D)) {
            m_PaddleRight->flash(0.4f, GP::getPalette().getColor(4));
            auto const pp = m_PaddleRight->getPosition();
            if (bp.y > pp.y && bp.y < pp.y + m_PaddleRight->getGlobalBounds().height) {
                if (bp.x > pp.x && bp.x < pp.x + m_PaddleRight->getGlobalBounds().width) {
                    auto oldV = m_ball->getVelocity();
                    float vxOffset = jt::Random::getFloatGauss(+10, 5);
                    if (oldV.y > 0) {
                        oldV.y = -oldV.y - jt::Random::getFloatGauss(+10, 5);
                    } else {
                        oldV.y = -200;
                    }
                    m_ball->setVelocity(jt::Vector2f { oldV.x + vxOffset, oldV.y });
                }
            }
        }

        if (m_ball->getPosition().y > GP::GetScreenSize().y + 32) {
            endGame();
        }
    }

    m_PaddleLeft->update(elapsed);
    m_PaddleRight->update(elapsed);

    m_background->update(elapsed);
    m_vignette->update(elapsed);
    m_overlay->update(elapsed);
}

void StateGame::doInternalDraw() const
{
    m_background->draw(getGame()->gfx().target());
    drawObjects();
    m_ball->draw();
    m_PaddleLeft->draw(getGame()->gfx().target());
    m_PaddleRight->draw(getGame()->gfx().target());
    m_vignette->draw(getGame()->gfx().target());
    m_hud->draw();
    m_overlay->draw(getGame()->gfx().target());
}

void StateGame::endGame()
{
    if (m_hasEnded) {
        // trigger this function only once
        return;
    }
    m_hasEnded = true;
    m_running = false;

    getGame()->getStateManager().switchState(std::make_shared<StateMenu>());
}
std::string StateGame::getName() const { return "Game"; }
