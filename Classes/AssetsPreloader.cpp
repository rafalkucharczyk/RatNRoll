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

string getFileExtension(const string &s)
{
    size_t i = s.rfind('.', s.length());

    if (i != string::npos) {
        return s.substr(i + 1, s.length() - i);
    }

    return "";
}

string dirname(const string &s)
{
    size_t i = s.rfind('.', s.length());

    if (i != string::npos) {
        return s.substr(0, i);
    }

    return "";
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

vector<string> AssetsPreloader::list()
{
    vector<string> result;

    string resourcesDir = FileUtils::getInstance()->fullPathForFilename("level_base.json");

    auto i = resourcesDir.find_last_of("/");
    resourcesDir = resourcesDir.substr(0, i);

    for (string assetsSubdir : {"earth", "items"}) {
        for (auto pngFilename :
             listDirectory(resourcesDir + std::string("/") + assetsSubdir, ".png")) {
            result.push_back(assetsSubdir + "/" + pngFilename);
        }
    }

    for (string animationName : {"rat", "digits"}) {
        result.push_back(string("animations/") + animationName + string("/skeleton.atlas"));
    }

    return result;
}

void AssetsPreloader::preload(const string &fileName)
{
    if (getFileExtension(fileName) == "png") {
        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(fileName);
        assert(texture);
        generateMipmap(*texture);
    }

    if (getFileExtension(fileName) == "atlas") {
        std::string jsonFile = dirname(fileName) + ".json";
        spine::SkeletonAnimation::createWithFile(jsonFile, fileName);
    }
}