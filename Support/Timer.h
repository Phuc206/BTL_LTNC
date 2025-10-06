#ifndef TIMER_H
#define TIMER_H

class Timer {
private:
    float timeSMax;
    float timeSCurrent;

public:
    Timer(float setTimeSMax, float setTimeSCurrent = 0.0f);
    void countUp(float dT);
    void countDown(float dT);
    void resetToZero();
    void resetToMax();
    bool timeSIsZero();
    bool timeSIsGreaterThanOrEqualTo(float timeSCheck);

    // Them cac phuong thuc moi
    void reset(float time) { timeSMax = time; timeSCurrent = time; } // Dat lai thoi gian toi da va hien tai
    float getTimeSCurrent() { return timeSCurrent; } // Them getter de debug
};

#endif
