#include "AssetsPreloader.h"

#include <cocos2d.h>
#include <spine/spine-cocos2dx.h>

#include <dirent.h>

using namespace std;
USING_NS_CC;

namespace
{
vector<string> listDirectory(string const &path, string const &pattern)
{
    vector<string> ret;

    DIR *dir = NULL;
    struct dirent *ent;

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') { // get rid of . and .. and hidden files
                string name = ent->d_name;

                if (name.size() && name.find(pattern) == string::npos) {
                    continue;
                }

                ret.push_back(ent->d_name);
            }
        }

        closedir(dir);
    }

    return ret;
}

void generateMipmap(Texture2D &texture)
{
    if (texture.getPixelsWide() == ccNextPOT(texture.getPixelsWide()) &&
        texture.getPixelsHigh() == ccNextPOT(texture.getPixelsHigh())) {
        texture.generateMipmap();
        Texture2D::TexParams texParams = {GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_EDGE};
        texture.setTexParameters(texParams);
    }
}
}

void AssetsPreloader::preload()
{
    string resourcesDir = FileUtils::getInstance()->fullPathForFilename("level_base.json");

    auto i = resourcesDir.find_last_of("/");
    resourcesDir = resourcesDir.substr(0, i);

    for (string assetsSubdir : {"earth", "items"}) {
        for (auto pngFilename :
             listDirectory(resourcesDir + std::string("/") + assetsSubdir, ".png")) {
            Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(
                assetsSubdir + "/" + pngFilename);
            assert(texture);
            generateMipmap(*texture);
        }
    }

    for (string animationName : {"rat", "digits"}) {
        spine::SkeletonAnimation::createWithFile(
            string("animations/") + animationName + string("/skeleton.json"),
            string("animations/") + animationName + string("/skeleton.atlas"));
    }
}
