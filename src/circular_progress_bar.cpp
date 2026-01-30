#include "circular_progress_bar.hpp"
#include <cmath>

CircularProgressBar::CircularProgressBar(float center_x, float center_y, float radius, float thickness)
    : center_xM(center_x)
    , center_yM(center_y)
    , radiusM(radius)
    , thicknessM(thickness)
    , progressM(0.0f)
{
    // Default colors - light gray background, orange progress (matching your image)
    background_colorM = {220, 220, 220, 255};
    progress_colorM = {220, 80, 30, 255};
}

CircularProgressBar::~CircularProgressBar() {
    // Nothing to clean up
}

void CircularProgressBar::set_progress(float progress) {
    // Clamp progress between 0 and 1
    if (progress < 0.0f) {
        progressM = 0.0f;
    } else if (progress > 1.0f) {
        progressM = 1.0f;
    } else {
        progressM = progress;
    }
}

void CircularProgressBar::reset() {
    progressM = 0.0f;
}

void CircularProgressBar::set_position(float x, float y) {
    center_xM = x;
    center_yM = y;
}

void CircularProgressBar::set_radius(float radius) {
    radiusM = radius;
}

void CircularProgressBar::set_thickness(float thickness) {
    thicknessM = thickness;
}

void CircularProgressBar::set_background_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    background_colorM = {r, g, b, a};
}

void CircularProgressBar::set_progress_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    progress_colorM = {r, g, b, a};
}

bool CircularProgressBar::draw_arc(SDL_Renderer* renderer, float start_angle, float end_angle, 
                                    const SDL_Color& color) {
    if (!renderer) {
        return false;
    }
    
    // Convert SDL_Color to SDL_FColor (0-255 range to 0.0-1.0 range)
    SDL_FColor fcolor;
    fcolor.r = color.r / 255.0f;
    fcolor.g = color.g / 255.0f;
    fcolor.b = color.b / 255.0f;
    fcolor.a = color.a / 255.0f;
    
    // Calculate the number of segments based on arc length for smooth rendering
    float arc_length = end_angle - start_angle;
    // More segments for smoother appearance - increased for better anti-aliasing
    int segments = static_cast<int>(arc_length * radiusM * 1.0f);
    if (segments < 20) segments = 20;  // Increased minimum segments for smoother circles
    
    float angle_step = arc_length / segments;
    
    // Calculate inner and outer radius
    float outer_radius = radiusM + thicknessM / 2.0f;
    float inner_radius = radiusM - thicknessM / 2.0f;
    
    // Number of feathering layers for anti-aliasing
    const int feather_layers = 3;
    const float feather_width = 1.5f;  // Width of feathering in pixels
    
    // Draw feathered outer edge (semi-transparent)
    for (int layer = 0; layer < feather_layers; ++layer) {
        float layer_radius = outer_radius + (layer * feather_width / feather_layers);
        float next_radius = outer_radius + ((layer + 1) * feather_width / feather_layers);
        float alpha_factor = 1.0f - ((layer + 1) / (float)(feather_layers + 1));
        
        SDL_FColor feather_color = fcolor;
        feather_color.a = fcolor.a * alpha_factor;
        
        for (int i = 0; i < segments; ++i) {
            float angle1 = start_angle + i * angle_step;
            float angle2 = start_angle + (i + 1) * angle_step;
            
            float x1_outer = center_xM + layer_radius * cosf(angle1);
            float y1_outer = center_yM + layer_radius * sinf(angle1);
            float x2_outer = center_xM + layer_radius * cosf(angle2);
            float y2_outer = center_yM + layer_radius * sinf(angle2);
            
            float x1_inner = center_xM + next_radius * cosf(angle1);
            float y1_inner = center_yM + next_radius * sinf(angle1);
            float x2_inner = center_xM + next_radius * cosf(angle2);
            float y2_inner = center_yM + next_radius * sinf(angle2);
            
            SDL_Vertex vertices[6];
            
            vertices[0].position.x = x1_outer;
            vertices[0].position.y = y1_outer;
            vertices[0].color = feather_color;
            
            vertices[1].position.x = x2_outer;
            vertices[1].position.y = y2_outer;
            vertices[1].color = feather_color;
            
            vertices[2].position.x = x1_inner;
            vertices[2].position.y = y1_inner;
            vertices[2].color = feather_color;
            
            vertices[3].position.x = x2_outer;
            vertices[3].position.y = y2_outer;
            vertices[3].color = feather_color;
            
            vertices[4].position.x = x2_inner;
            vertices[4].position.y = y2_inner;
            vertices[4].color = feather_color;
            
            vertices[5].position.x = x1_inner;
            vertices[5].position.y = y1_inner;
            vertices[5].color = feather_color;
            
            if (!SDL_RenderGeometry(renderer, nullptr, vertices, 6, nullptr, 0)) {
                return false;
            }
        }
    }
    
    // Draw main arc (solid)
    for (int i = 0; i < segments; ++i) {
        float angle1 = start_angle + i * angle_step;
        float angle2 = start_angle + (i + 1) * angle_step;
        
        float x1_outer = center_xM + outer_radius * cosf(angle1);
        float y1_outer = center_yM + outer_radius * sinf(angle1);
        float x2_outer = center_xM + outer_radius * cosf(angle2);
        float y2_outer = center_yM + outer_radius * sinf(angle2);
        
        float x1_inner = center_xM + inner_radius * cosf(angle1);
        float y1_inner = center_yM + inner_radius * sinf(angle1);
        float x2_inner = center_xM + inner_radius * cosf(angle2);
        float y2_inner = center_yM + inner_radius * sinf(angle2);
        
        SDL_Vertex vertices[6];
        
        vertices[0].position.x = x1_outer;
        vertices[0].position.y = y1_outer;
        vertices[0].color = fcolor;
        
        vertices[1].position.x = x2_outer;
        vertices[1].position.y = y2_outer;
        vertices[1].color = fcolor;
        
        vertices[2].position.x = x1_inner;
        vertices[2].position.y = y1_inner;
        vertices[2].color = fcolor;
        
        vertices[3].position.x = x2_outer;
        vertices[3].position.y = y2_outer;
        vertices[3].color = fcolor;
        
        vertices[4].position.x = x2_inner;
        vertices[4].position.y = y2_inner;
        vertices[4].color = fcolor;
        
        vertices[5].position.x = x1_inner;
        vertices[5].position.y = y1_inner;
        vertices[5].color = fcolor;
        
        if (!SDL_RenderGeometry(renderer, nullptr, vertices, 6, nullptr, 0)) {
            return false;
        }
    }
    
    // Draw feathered inner edge (semi-transparent)
    for (int layer = 0; layer < feather_layers; ++layer) {
        float layer_radius = inner_radius - (layer * feather_width / feather_layers);
        float next_radius = inner_radius - ((layer + 1) * feather_width / feather_layers);
        float alpha_factor = 1.0f - ((layer + 1) / (float)(feather_layers + 1));
        
        SDL_FColor feather_color = fcolor;
        feather_color.a = fcolor.a * alpha_factor;
        
        for (int i = 0; i < segments; ++i) {
            float angle1 = start_angle + i * angle_step;
            float angle2 = start_angle + (i + 1) * angle_step;
            
            float x1_outer = center_xM + layer_radius * cosf(angle1);
            float y1_outer = center_yM + layer_radius * sinf(angle1);
            float x2_outer = center_xM + layer_radius * cosf(angle2);
            float y2_outer = center_yM + layer_radius * sinf(angle2);
            
            float x1_inner = center_xM + next_radius * cosf(angle1);
            float y1_inner = center_yM + next_radius * sinf(angle1);
            float x2_inner = center_xM + next_radius * cosf(angle2);
            float y2_inner = center_yM + next_radius * sinf(angle2);
            
            SDL_Vertex vertices[6];
            
            vertices[0].position.x = x1_outer;
            vertices[0].position.y = y1_outer;
            vertices[0].color = feather_color;
            
            vertices[1].position.x = x2_outer;
            vertices[1].position.y = y2_outer;
            vertices[1].color = feather_color;
            
            vertices[2].position.x = x1_inner;
            vertices[2].position.y = y1_inner;
            vertices[2].color = feather_color;
            
            vertices[3].position.x = x2_outer;
            vertices[3].position.y = y2_outer;
            vertices[3].color = feather_color;
            
            vertices[4].position.x = x2_inner;
            vertices[4].position.y = y2_inner;
            vertices[4].color = feather_color;
            
            vertices[5].position.x = x1_inner;
            vertices[5].position.y = y1_inner;
            vertices[5].color = feather_color;
            
            if (!SDL_RenderGeometry(renderer, nullptr, vertices, 6, nullptr, 0)) {
                return false;
            }
        }
    }
    
    return true;
}

bool CircularProgressBar::draw(SDL_Renderer* renderer) {
    if (!renderer) {
        return false;
    }
    
    // Start angle at top (-90 degrees = -PI/2 radians)
    float start_angle = -M_PI / 2.0f;
    
    // Full circle is 2*PI radians
    float full_circle = 2.0f * M_PI;
    
    // Draw background circle (full circle)
    if (!draw_arc(renderer, start_angle, start_angle + full_circle, background_colorM)) {
        return false;
    }
    
    // Draw progress arc
    if (progressM > 0.0f) {
        float progress_angle = start_angle + (full_circle * progressM);
        if (!draw_arc(renderer, start_angle, progress_angle, progress_colorM)) {
            return false;
        }
    }
    
    return true;
}
