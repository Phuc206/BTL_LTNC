#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H


#include <SDL_mixer.h>
#include <map>
#include <string>

class AudioManager {
public:
    static void init();               // Khoi tao am thanh
    static void close();              // Giai phong tai nguyen
    static void playMusic(const std::string& file, int loops = -1);  // Phat nhac nen
    static void playSound(const std::string& file);                  // Phat hieu ung
    static void setVolume(int volume);  // Dieu chinh am luong
    static Mix_Chunk* getSound(const std::string& path);

private:
    static std::map<std::string, Mix_Chunk*> soundEffects;
    static Mix_Music* backgroundMusic;
};

#endif
