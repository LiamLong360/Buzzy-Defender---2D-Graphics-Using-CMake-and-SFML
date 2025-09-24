/*
Author: Liam Long
Class: ECE4122 
Last Date Modified: 9/22/25
Description:
This header file defines the ECE_LaserBlast class, which represents a single laser
projectile in the Buzzy Defender game. The class inherits from sf::Sprite so it
can be drawn and transformed directly, and it provides methods to control
movement, velocity, and screen-boundary checks for both player and enemy shots.
*/ 

#include <SFML/Graphics.hpp>                                                    // provides classes like sf::Sprite
                                                                                //                       sf::Texture
                                                                                //                       sf::RenderWindow
// using namespace for readbility
using namespace sf;     


/*
 * Class: ECE_LaserBlast
 * Purpose: Represent an individual laser blast fired by either the player or an
 *          enemy. Provides velocity control and position updates each frame.
 * Inherits:
 *      sf::Sprite - to use SFML's drawing and transformation features.
 */
class ECE_LaserBlast : public Sprite
{
public:
    /*
     *  Purpose: constructor method for ECE_LaserBlast with a given texture and source flag.
     *  Input(s):
     *      const Texture& texture - reference to texture to fraw for the blast
     *      bool fromPlayer        - true if from player, false if from enemy
     *  Output:
     *      None (constructor)
     */
    ECE_LaserBlast(const Texture& texture, bool fromPlayer);
    
    /*
     *  Purpose: set the laser's velocity.
     *  Input(s):
     *      Vector2f v – desired velocity (pixels per second)
     *  Output:
     *      None
     */
    void setVelocity(Vector2f v);
    
    /*
     *  Purpose: get the laser's velocity.
     *  Input(s):
     *      None
     *  Output:
     *      Vector2f – current velocity (pixels per second)
     */
    Vector2f getVelocity() const;
    
    /*
     * Purpose: Update the laser’s position based on elapsed time.
     * Input(s):
     *      float dt – seconds since last frame
     * Output:
     *      None
     * Notes:
     *      Uses Sprite class's inherited move() to add offset = velocity * dt
     */    
    void update(float dt);
    
    /*
     * Purpose: Identify whether the laser was fired by the player.
     * Input(s):
     *      None
     * Output:
     *      bool – true if fired from  player, false if enemy
     */
    bool fromPlayer() const;
    
    /*
     * Purpose: Check if the laser has moved off the vertical bounds of the window.
     * Input(s):
     *      float windowHeight – total height of the game window in pixels
     * Output:
     *      bool – true if the laser is completely outside the screen
     */
    bool isOffScreen(float windowHeight) const;
    
private:
    Vector2f m_vel{0.f, 0.f};   // laser's velocity in pixels per second
    bool m_fromPlayer = true;   // flag - true if from player, false if enemy
};
