#ifndef CIRCULAR_PROGRESS_BAR_H
#define CIRCULAR_PROGRESS_BAR_H

#include <SDL3/SDL.h>

class CircularProgressBar {
public:
    CircularProgressBar(float center_x, float center_y, float radius, float thickness);

    // Set the progress (0.0 to 1.0)
    void set_progress(float progress);

    // Reset the progress to 0
    void reset();

    // Draw the progress bar
    bool draw(SDL_Renderer* renderer);

    // Setters for customization
    void set_position(float x, float y);
    void set_radius(float radius);
    void set_thickness(float thickness);
    void set_background_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void set_progress_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

private:
    float center_xM;
    float center_yM;
    float radiusM;
    float thicknessM;
    float progressM;

    // Colors
    SDL_Color background_colorM;
    SDL_Color progress_colorM;

    // Helper method to draw an arc
    bool draw_arc(SDL_Renderer* renderer, float start_angle, float end_angle, 
                  const SDL_Color& color);
};

#endif // CIRCULAR_PROGRESS_BAR_H
