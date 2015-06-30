#ifndef __LEVEL_CUSTOMIZATION_H__
#define __LEVEL_CUSTOMIZATION_H__

#include <string>

class LevelCustomization
{
  public:
    virtual ~LevelCustomization() {}

  public:
    virtual std::string getRubeJsonFileName() const = 0;

    virtual float getItemDropInterval() const = 0; // in seconds
};

class LevelTutorial : public LevelCustomization
{
  public:
    std::string getRubeJsonFileName() const { return "level_tutorial.json"; }

    float getItemDropInterval() const { return 1.0; }
};

class Level01 : public LevelCustomization
{
  public:
    std::string getRubeJsonFileName() const { return "level_01.json"; }

    float getItemDropInterval() const { return 3.0; }
};

#endif // __LEVEL_LOGIC_H__
