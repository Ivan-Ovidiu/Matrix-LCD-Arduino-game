#ifndef BULLET_H
#define BULLET_H


class Bullet {
public:
    int x;
    int y;
    bool active;
    unsigned long lastMoveTime;
    int moveSpeed;


    Bullet() : x(0), y(0), active(false), lastMoveTime(0), moveSpeed(1000) {}

    void spawn(int spawnX, int spawnY) {
        x = spawnX;
        y = spawnY;
        active = true;
        lastMoveTime = millis();
    }

    void deactivate() {
        active = false;
    }

    bool isActive() const {
        return active;
    }

    int getX() const { 
        return x; 
    }
    int getY() const { 
        return y; 
    }

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }


//debouncing functions for the bullet speed
    bool shouldMove(unsigned long currentTime) {
        return (currentTime - lastMoveTime >= moveSpeed);
    }

    void updateMoveTime(unsigned long currentTime) {
        lastMoveTime = currentTime;
    }

//actual speed of the bullet
    void setMoveSpeed(int speed) {
        moveSpeed = speed;
    }
};

#endif
