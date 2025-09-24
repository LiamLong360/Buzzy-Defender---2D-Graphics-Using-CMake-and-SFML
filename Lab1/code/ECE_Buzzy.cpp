#include "ECE_Buzzy.h"
#include <SFML/Graphics.hpp>
#include <algorithm>    // for std::clamp

using namespace sf;

/*
 * Purpose:
 *      Constructs a Buzzy player sprite and initializes its base sf::Sprite
 *      with the provided texture so it can be drawn on the screen.
 * Input(s):
 *      const Texture& texture – read-only reference to the texture image
 *                                representing the player character.
 * Output:
 *      None (constructor).
 * Notes:
 *      Uses 'explicit' to prevent unintended implicit conversions when
 *      passing a Texture to the constructor.
 */
ECE_Buzzy::ECE_Buzzy(const Texture& texture) // uses scope resolution operator "::" - only this constructor belogns to class ECE_Buzzy
                                                 // constructor that uses a reference to a texture - does NOT change
: Sprite(texture)   // call base-class constructor
                    // When creating an ECE_Buzzy, first construct its Sprite part, passing it the texture so the sprite is ready to display that image
{

    // scale/center
    setOrigin(getLocalBounds().width / 2.f,   // Sprite's texture width in pixels
              getLocalBounds().height / 2.f); // Sprite's texture height in pixels
              
    auto b = getLocalBounds();
    
    // Scale so that the sprite fits roughly inside 100x100 pixels
    float scaleX = 100.f / b.width;
    float scaleY = 100.f / b.height;
    setScale(scaleX, scaleY);
}

/*
 * Purpose:
 *      Scales the Buzzy sprite so it fits within a target fraction of the window size.
 * Input(s):
 *      const sf::Vector2u& windowSize - current window dimensions in pixels
 *      float boxFracW - fraction of the window width to occupy (default 0.1f)
 *      float boxFracH - fraction of the window height to occupy (default 0.1f)
 * Output:
 *      None
 */
void ECE_Buzzy::scaleForWindow(const Vector2u& windowSize,
                               float boxFracW,
                               float boxFracH)
{
    const auto b = getLocalBounds();
                                                                                // target box in pixels (fraction of window size)
    const float targetW = windowSize.x * boxFracW;
    const float targetH = windowSize.y * boxFracH;
                                                                                // uniform scale so the sprite fits inside the target box
    const float sx = targetW / b.width;
    const float sy = targetH / b.height;
    const float s  = std::min(sx, sy);                                          // make texture a square & choose minumum of width vs height

    setScale(s, s);
}

/*
 * Purpose:
 *      Sets the horizontal movement speed of the Buzzy sprite.
 * Input(s):
 *      float s - speed in pixels per second
 * Output:
 *      None
 */
void ECE_Buzzy::setSpeed(float s)
    { 
        m_speed = s;
    }
    
/*
 * Purpose:
 *      Retrieves the current horizontal movement speed.
 * Input(s):
 *      None
 * Output:
 *      float - speed in pixels per second
 */
float ECE_Buzzy::getSpeed() const
    {
        return m_speed;
    }

/*
 * Purpose:
 *      Updates Buzzy's position based on keyboard input and elapsed time.
 * Input(s):
 *      float dt - time elapsed since last update (seconds)
 *      float windowWidth - width of the game window (pixels)
 * Output:
 *      None
 */
void ECE_Buzzy::update(float dt, float windowWidth)
{
    float dx = 0.f;
    if (Keyboard::isKeyPressed(Keyboard::Left))
    {
        dx -= m_speed * dt; // pixels/sec * sec = # pixels to move (-x = left)
    }
    if (Keyboard::isKeyPressed(Keyboard::Right))
    {
        dx += m_speed * dt; // pixels/sec * sec = # pixels to move (+x = right)
    }

    Vector2f pos = getPosition(); // 2D vector                      // clamp(value, min, max) restricts value to stay between min & max
    pos.x = std::clamp(pos.x + dx,                                      // pos.x + dx - new horizontal position after movement
                        getGlobalBounds().width / 2.f,                  // getGlobalBounds: returns current bounding rectangle of a Sprite in global coordinates
                                                                        // keep left half of sprite on screen
                        windowWidth - getGlobalBounds().width / 2.f);   // keep right half of sprite on screen
    setPosition(pos); // update Sprite's position
}