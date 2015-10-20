#include "InitialLayer.h"

USING_NS_CC;

InitialLayer::InitialLayer()
    : menuHelper({{{0.2, 0.275}, 0.1, "tutorial"},
                  {{0.5, 0.625}, 0.35, "play"},
                  {{0.4, 0.275}, 0.1, "settings"},
                  {{0.6, 0.275}, 0.1, "gamecenter"},
                  {{0.8, 0.275}, 0.1, "about"}},

                 std::bind(&InitialLayer::itemClicked, this, std::placeholders::_1))
{
}

bool InitialLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    // TODO: remove font testing code from final release
    constexpr auto labelTestMode = false;

    if (labelTestMode) {
        // labels contains paris of <text to display, height of a single line>
        std::pair<std::string, float> labels[] = {
            {"ABCDEFGHJIKLMNOPRSTUWYZ\n"
                "1234567890 _?.,!", 0.03 },
            {"ROTATE THE WHEEL AND KEEP\n"
                "RAT ON TOP OF IT.\n\n"
                "THE MORE YOU ROLL\n"
                "THE MORE YOU SCORE!", 0.03 },
            {"COLLECT VIALS TO\n"
                "SPEED UP THE RAT!", 0.03},
            {"AVOID HAMBURGERS,\n"
                "THEY SLOW YOU DOWN!", 0.03},
            {"GOOSE SENDS YOU INTO THE AIR.\n"
                "THEN ROLL AS FAST AS YOU CAN\n"
                "TO GET TONS OF POINTS!", 0.03},
            {"RAT 'N' ROLL?", 0.04 },
        };
        float y = 0.1;
        float linesGap = 0.07;

        for (auto label: labels) {
            std::string &labelStr = label.first;
            auto numLines = std::count(labelStr.begin(), labelStr.end(), '\n') + 1;
            auto height = label.second*numLines;
            auto testLabel = Label::createWithTTF(labelStr, "fonts/rat.ttf", 40,
                                                  Size::ZERO, TextHAlignment::CENTER);
            testLabel->setColor(Color3B::BLACK);

            MenuHelper::positionNode(*testLabel, {0.5, y}, height);
            y += height + linesGap;

            addChild(testLabel);
        }
    } else {
        addChild(menuHelper.getMenu());
    }
    return true;
}
