#pragma once

#include <raylib.h>

enum class eDirection {
    Down = 0,
    Right,
    Up,
    Left
};

enum class eAnimType {
    Idle = 0,
    Walk,
    Run,
    Max
};

class SpriteAnimation {
public:
    Texture2D texture;
    int rows;
    int cols;
    int size;
    float frameTime;
    int frame_span;
    bool row_based;

    // Runtime state
    eDirection direction;
    eAnimType animType;
    int frame;
    float timer;

    bool loop; // if false, play once and set finished
    bool finished; // true when non-looping animation reached last frame

    SpriteAnimation()
        : rows(1)
        , cols(1)
        , size(0)
        , frameTime(0.2f)
        , frame_span(1)
        , row_based(true)
        , direction(eDirection::Down)
        , animType(eAnimType::Idle)
        , frame(0)
        , timer(0.0f)
        , loop(true)
        , finished(true)
    {
    }

    void init(Texture2D tex, int r, int c, int s, float ft, int span = 1, bool row_anim = true, bool loop = true)
    {
        texture = tex;
        rows = r;
        cols = c;
        size = s;
        frameTime = ft;
        frame_span = span;
        row_based = row_anim;
        loop = loop;
        frame = 0;
        timer = 0.0f;
        finished = false;
    }

    void reset()
    {
        frame = 0;
        timer = 0.0f;
        finished = false;
    }

    bool is_finished() const
    {
        return finished;
    }

    void update(float delta, bool moving)
    {
        timer += delta;
        if (timer >= frameTime) {
            timer -= frameTime;
            frame++;

            int frame_limit = row_based ? cols : rows;
            int maxFrames = frame_limit / frame_span;
            if (frame >= maxFrames)
                frame = 0;
        }
    }

    void draw(float posX, float posY, float drawWidth, float drawHeight)
    {
        int dirIndex = static_cast<int>(direction);
        int row = 0;
        int col = 0;

        // Depending on layout type
        if (row_based) {
            row = dirIndex < rows ? dirIndex : 0;
            col = frame;
        } else {
            col = dirIndex < cols ? dirIndex : 0;
            row = frame;
        }

        Rectangle src = {
            static_cast<float>((col * frame_span) * size),
            static_cast<float>(row * size),
            (float)(frame_span * size),
            (float)size
        };

        // adjust for span
        float adjustedX = posX - ((frame_span - 1) * drawWidth * 0.5f);
        float adjustedY = posY;

        Rectangle dest = {
            adjustedX,
            adjustedY, // bottom of sprite sits on the tile
            drawWidth * frame_span,
            drawHeight
        };
        /*Rectangle dest = {
            posX,
            posY, // bottom of sprite sits on the tile
            drawWidth,
            drawHeight
        };*/

        DrawTexturePro(texture, src, dest, { 0, 0 }, 0.0f, WHITE);
    }
};
