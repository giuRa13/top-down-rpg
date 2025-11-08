#pragma once

#include <raylib.h>

enum class eDirection { Down = 0,
    Up,
    Right
};

enum class eAnimType { Idle = 0,
    Walk,
    Run,
    Max
};

class SpriteAnim {
public:
    Texture2D texture;
    int rows;
    int cols; // usually 3 for down/up/right
    int framesPerCol;
    float frameTime;

    // Runtime state
    eDirection direction;
    eAnimType animType;
    int frame;
    float timer;
    bool flip;

    SpriteAnim()
        : rows(0)
        , cols(0)
        , framesPerCol(0)
        , frameTime(0.2f)
        , direction(eDirection::Down)
        , animType(eAnimType::Idle)
        , frame(0)
        , timer(0.0f)
        , flip(false)
    {
    }

    void init(Texture2D tex, int r, int c, int fpc, float ft)
    {
        texture = tex;
        rows = r;
        cols = c;
        framesPerCol = fpc;
        frameTime = ft;
    }

    void update(bool moving)
    {
        animType = moving ? eAnimType::Walk : eAnimType::Idle;
        timer += GetFrameTime();
        if (timer >= frameTime) {
            timer = 0.0f;
            frame++;
            if (frame >= framesPerCol)
                frame = 0;
        }
    }

    void draw(float posX, float posY, float drawWidth, float drawHeight)
    {
        int row = static_cast<int>(direction);
        int col = frame;

        Rectangle src = {
            static_cast<float>(row * 64),
            static_cast<float>(col * 64),
            flip ? -64.0f : 64.0f,
            64.0f
        };

        // Shift sprite so its bottom aligns with the player's feet
        Rectangle dest = {
            posX,
            posY - drawHeight + 16, // bottom of sprite sits on the tile
            drawWidth,
            drawHeight
        };

        DrawTexturePro(texture, src, dest, { 0, 0 }, 0.0f, WHITE);
    }
};
