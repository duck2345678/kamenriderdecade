// Auto-generated Decade Sprite Frame Rectangles for SE102 Engine
#pragma once
#include <windows.h>
#include <vector>
#include <string>

struct FrameData {
    RECT rect;
    float duration; // seconds
};

struct AnimationData {
    std::string name;
    std::wstring texturePath;
    bool isLoop;
    std::vector<FrameData> frames;
};

inline std::vector<AnimationData> GetDecadeAnimationConfigs() {
    std::vector<AnimationData> anims;

    {
        AnimationData a; a.name = "IDLE"; a.texturePath = L"Assets/Textures/decade_idle.png"; a.isLoop = true;
        a.frames.push_back({ RECT{ 0, 5, 118, 283 }, 0.125f });
        a.frames.push_back({ RECT{ 190, 2, 312, 283 }, 0.125f });
        a.frames.push_back({ RECT{ 375, 0, 499, 283 }, 0.125f });
        a.frames.push_back({ RECT{ 542, 0, 667, 283 }, 0.125f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "WALK"; a.texturePath = L"Assets/Textures/decade_walk.png"; a.isLoop = true;
        a.frames.push_back({ RECT{ 20, 17, 193, 295 }, 0.100f });
        a.frames.push_back({ RECT{ 277, 15, 408, 295 }, 0.100f });
        a.frames.push_back({ RECT{ 470, 11, 583, 295 }, 0.100f });
        a.frames.push_back({ RECT{ 659, 12, 812, 295 }, 0.100f });
        a.frames.push_back({ RECT{ 892, 19, 1077, 296 }, 0.100f });
        a.frames.push_back({ RECT{ 1145, 17, 1283, 296 }, 0.100f });
        a.frames.push_back({ RECT{ 1349, 11, 1456, 296 }, 0.100f });
        a.frames.push_back({ RECT{ 1564, 15, 1696, 296 }, 0.100f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "JUMP"; a.texturePath = L"Assets/Textures/decade_jump.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 20, 34, 235, 352 }, 0.167f });
        a.frames.push_back({ RECT{ 327, 34, 558, 301 }, 0.167f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "BLOCK"; a.texturePath = L"Assets/Textures/decade_block.png"; a.isLoop = true;
        a.frames.push_back({ RECT{ 11, 17, 194, 345 }, 0.167f });
        a.frames.push_back({ RECT{ 304, 36, 490, 345 }, 0.167f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "ATTACK_J"; a.texturePath = L"Assets/Textures/decade_attack_j.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 38, 35, 154, 222 }, 0.083f });
        a.frames.push_back({ RECT{ 227, 44, 351, 222 }, 0.083f });
        a.frames.push_back({ RECT{ 425, 37, 551, 221 }, 0.083f });
        a.frames.push_back({ RECT{ 607, 39, 742, 221 }, 0.083f });
        a.frames.push_back({ RECT{ 780, 47, 954, 221 }, 0.083f });
        a.frames.push_back({ RECT{ 964, 50, 1135, 220 }, 0.083f });
        a.frames.push_back({ RECT{ 1199, 35, 1297, 223 }, 0.083f });
        a.frames.push_back({ RECT{ 1362, 13, 1535, 230 }, 0.083f });
        a.frames.push_back({ RECT{ 1583, 45, 1709, 222 }, 0.083f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "ATTACK_K"; a.texturePath = L"Assets/Textures/decade_attack_k.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 30, 91, 287, 358 }, 0.083f });
        a.frames.push_back({ RECT{ 321, 69, 619, 358 }, 0.083f });
        a.frames.push_back({ RECT{ 645, 69, 994, 358 }, 0.083f });
        a.frames.push_back({ RECT{ 1031, 85, 1355, 356 }, 0.083f });
        a.frames.push_back({ RECT{ 1438, 4, 1710, 358 }, 0.083f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "ATTACK_L"; a.texturePath = L"Assets/Textures/decade_attack_l.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 28, 32, 183, 352 }, 0.100f });
        a.frames.push_back({ RECT{ 266, 44, 496, 352 }, 0.100f });
        a.frames.push_back({ RECT{ 566, 44, 794, 352 }, 0.100f });
        a.frames.push_back({ RECT{ 867, 20, 1043, 352 }, 0.100f });
        a.frames.push_back({ RECT{ 1136, 18, 1300, 354 }, 0.100f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "DIMENSION_KICK"; a.texturePath = L"Assets/Textures/decade_dimension_kick.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 38, 23, 126, 245 }, 0.100f });
        a.frames.push_back({ RECT{ 182, 23, 269, 245 }, 0.100f });
        a.frames.push_back({ RECT{ 324, 23, 411, 245 }, 0.100f });
        a.frames.push_back({ RECT{ 462, 23, 554, 245 }, 0.100f });
        a.frames.push_back({ RECT{ 598, 23, 684, 245 }, 0.100f });
        a.frames.push_back({ RECT{ 738, 30, 877, 236 }, 0.100f });
        a.frames.push_back({ RECT{ 932, 42, 1052, 224 }, 0.100f });
        a.frames.push_back({ RECT{ 1105, 45, 1267, 162 }, 0.100f });
        a.frames.push_back({ RECT{ 1315, 42, 1499, 178 }, 0.100f });
        a.frames.push_back({ RECT{ 1526, 19, 1694, 198 }, 0.100f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "HURT"; a.texturePath = L"Assets/Textures/decade_hurt.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 2, 28, 204, 315 }, 0.167f });
        a.frames.push_back({ RECT{ 292, 48, 512, 314 }, 0.167f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "DEFEAT"; a.texturePath = L"Assets/Textures/decade_defeat.png"; a.isLoop = false;
        a.frames.push_back({ RECT{ 21, 14, 104, 124 }, 0.167f });
        a.frames.push_back({ RECT{ 155, 27, 269, 123 }, 0.167f });
        a.frames.push_back({ RECT{ 297, 56, 414, 122 }, 0.167f });
        a.frames.push_back({ RECT{ 436, 71, 558, 122 }, 0.167f });
        a.frames.push_back({ RECT{ 579, 84, 708, 125 }, 0.167f });
        a.frames.push_back({ RECT{ 728, 14, 853, 125 }, 0.167f });
        anims.push_back(a);
    }
    {
        AnimationData a; a.name = "POSE"; a.texturePath = L"Assets/Textures/decade_pose.png"; a.isLoop = true;
        a.frames.push_back({ RECT{ 13, 37, 77, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 107, 37, 191, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 211, 37, 310, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 338, 37, 403, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 440, 37, 505, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 553, 37, 636, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 659, 37, 759, 187 }, 0.250f });
        a.frames.push_back({ RECT{ 795, 35, 859, 187 }, 0.250f });
        anims.push_back(a);
    }
    return anims;
}
