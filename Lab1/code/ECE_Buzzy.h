/*
Author: Liam Long
Class: ECE4122 
Last Date Modified: 9/22/25
Description:
ECE_Buzzy class. Represents the player-controlled character ("Buzzy") as a
sprite. Inherits from Sprite to gain position, drawing, and transformation
methods. Includes functions for scaling to the window size, setting and
retrieving movement speed, and updating position based on keyboard input.
*/

#include <SFML/Graphics.hpp>    // Provides the sf::Sprite, sf::Texture, and related graphics classes

//using namespace for readability
using namespace sf;

class ECE_Buzzy : public Sprite                 // Define new class named ECE_Buzzy that inherits from base class Sprite from SFML
{
public:
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
    explicit ECE_Buzzy(const Texture& texture); // Constructor declaration, texture can't be changed, and must be of form: ECE_Buzzy buzzy(tex);
    
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
    void scaleForWindow(const sf::Vector2u& windowSize,                         // allow scaling to match window size
                        float boxFracW = 0.1f,                                  // width of buzzy texture is 10% of screen width
                        float boxFracH = 0.1f);                                 // width of buzzy texture is 10% of screen height
    
    /*
     * Purpose:
     *      Sets the horizontal movement speed of the Buzzy sprite.
     * Input(s):
     *      float s - speed in pixels per second
     * Output:
     *      None
     */
    void setSpeed(float s);
    
    /*
     * Purpose:
     *      Retrieves the current horizontal movement speed.
     * Input(s):
     *      None
     * Output:
     *      float - speed in pixels per second
     */
    float getSpeed() const;

    /*
     * Purpose:
     *      Updates Buzzy's position based on keyboard input and elapsed time.
     * Input(s):
     *      float dt - time elapsed since last update (seconds)
     *      float windowWidth - width of the game window (pixels)
     * Output:
     *      None
     */
    void update(float dt, float windowWidth);

private:
    float m_speed = 450.f; // Horizonal speed in pixels per second
};