#include <iostream>
#include <optional>
#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

using Point2D = sf::Vector2f;

// TODO: (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<sf::Vector2f>& pts, float t) { 
    static std::vector<Point2D> tmp;
    if (pts.size() <= 1) {
        return pts[0];
    }
    tmp.clear();
    for (size_t x = 0; x < pts.size()-1; x++)
        tmp.push_back(pts[x] * (1.0f - t) + pts[x + 1] * t);
    while (tmp.size() > 1){
        for (size_t x = 0; x < tmp.size()-1; x++)
            tmp[x] = tmp[x] * (1.0f - t) + tmp[x + 1] * t;
        tmp.pop_back();
    }
    return tmp[0];
}


// TODO: (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t) {
    static std::vector<Point2D> tmp(3);
    tmp[0] = pts[1] - pts[0];
    tmp[1] = pts[2] - pts[1];
    tmp[2] = pts[3] - pts[2];
    return (3.0f * std::pow(1.0f - t, 2.0f) * tmp[0]) + (6.0f * (1.0f - t) * t * tmp[1]) + (3.0f * std::pow(t, 2.0f) * tmp[2]);
}

// TODO: (Part 1) Store four control points for the curve.
std::vector<Point2D> control_points = {Point2D(100, 100), Point2D(200, 200), Point2D(300, 100), Point2D(400, 200)};
size_t MAX_SAMPLE_POINTS = 100;
// TODO: (Part 2) Track animation time for the square moving along the curve.
float animation_time = 0.0f;
// TODO: (Part 3) Track the index of the control point being dragged.
int dragged_control_point_index = -1;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // TODO: (Part 3) On left-click, select the closest control point
            // using mouse->position and start dragging it.
            for (size_t i = 0; i < control_points.size(); i++) {
                static Point2D tmp;
                tmp = static_cast<Point2D>(mouse->position) - control_points[i];
                if (std::sqrt(tmp.x * tmp.x + tmp.y * tmp.y) < 10) {
                    dragged_control_point_index = i;
                    return;
                }
            }
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // TODO: (Part 3) On left-button release, stop dragging.
            dragged_control_point_index = -1;
            return;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            // TODO: (Part 3) Move the selected control point to mouse->position.
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
            if (dragged_control_point_index != -1) {
                control_points[dragged_control_point_index] = static_cast<Point2D>(mouse->position);
                return;
            }
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
            if (key->code == sf::Keyboard::Key::Equal || key->code == sf::Keyboard::Key::Add) {
                
                
            } else if (key->code == sf::Keyboard::Key::Hyphen || key->code == sf::Keyboard::Key::Subtract) {
                
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======
    // Sample the curve
    sf::VertexArray graph(sf::PrimitiveType::LineStrip, MAX_SAMPLE_POINTS);
    for (size_t i = 0; i < MAX_SAMPLE_POINTS; i++) {
        graph[i].position = getPoint(control_points, i / static_cast<float>(MAX_SAMPLE_POINTS));
        graph[i].color = sf::Color::Yellow;
    }
    window.draw(graph);

    for (size_t i = 0; i < control_points.size(); i++) {
        static sf::CircleShape circle(5);
        circle.setOrigin({circle.getRadius(), circle.getRadius()});
        circle.setPosition(control_points[i]);
        circle.setFillColor(sf::Color::Blue);
        window.draw(circle);
    }

    // ====== ====== ======
    // TODO: (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    static sf::RectangleShape square(sf::Vector2f(10, 10));
    static float direction = 1;
    square.setOrigin({square.getSize().x / 2, square.getSize().y / 2});
    square.setPosition(getPoint(control_points, animation_time));
    Point2D slope = getSlope(control_points, animation_time);
    square.setRotation(sf::radians(std::atan2(slope.y, slope.x)));
    square.setFillColor(sf::Color::Red);
    window.draw(square);
    animation_time += direction / static_cast<float>(MAX_SAMPLE_POINTS);
    if (animation_time >= 1.0f || animation_time <= 0.0f) {
        direction *= -1;
    }

    // ====== ====== ======
    // TODO: (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======
    for (size_t i = 0; i < control_points.size() - 1; i += 2) {
        static sf::Vertex line[2];
        line[0].position = control_points[i];
        line[1].position = control_points[i + 1];
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }

    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
