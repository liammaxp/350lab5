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
    float r = 1.0- t;
    float r1 = r*r*r;
    float r2 = 3.0f*r*r*t;
    float r3 = 3.0f*r*t*t;
    float r4 = t*t*t;
    return r1*pts[0] + r2*pts[1] + r3*pts[2]+r4*pts[3]; 
}

// TODO: (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<sf::Vector2f>& pts, float t) { 
    float r = 1.0f - t;
    return 3.0f * r * r * (pts[1] - pts[0]) + 6.0f * r * t * (pts[2] - pts[1]) + 3.0f * t * t * (pts[3] - pts[2]);
    return Point2D{}; 
}

// TODO: (Part 1) Store four control points for the curve.
std::vector<sf::Vector2f> points = {
    {100.0f,600.0f},{250.0f,100.0f},
    {550.0f,100.0f},{700.0f,600.0f}
};
// TODO: (Part 2) Track animation time for the square moving along the curve.
float aniTime = 0.0f;
// TODO: (Part 3) Track the index of the control point being dragged.
int selectedPoint = -1;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } 
        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // TODO: (Part 3) On left-click, select the closest control point
            // using mouse->position and start dragging it.
            if (mouse->button == sf::Mouse::Button::Left) {
                float closestDistance = 1000000.0f;
                for (int i = 0; i < points.size(); i++) {
                    float dx = points[i].x - mouse->position.x;
                    float dy = points[i].y - mouse->position.y;
                    float distance = dx * dx + dy * dy;
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        selectedPoint = i;
                    }
                }
            }
        } 
        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // TODO: (Part 3) On left-button release, stop dragging.
            if (mouse->button == sf::Mouse::Button::Left) {
                selectedPoint = -1;
            }
        } 
        else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            // TODO: (Part 3) Move the selected control point to mouse->position.
            if (selectedPoint != -1) {
                Point2D oldPosition = points[selectedPoint];
                Point2D newPosition = { static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y)
                };
                Point2D moveAmount = newPosition - oldPosition;
                points[selectedPoint] = newPosition;
                // Moving a shared endpoint
                if (selectedPoint > 0 &&
                    selectedPoint < static_cast<int>(points.size()) - 1 &&
                    selectedPoint % 3 == 0) {
                    points[selectedPoint - 1] += moveAmount;
                    points[selectedPoint + 1] += moveAmount;
                }
                // Moving the control point before a shared endpoint
                else if (selectedPoint % 3 == 2 && selectedPoint + 2 < static_cast<int>(points.size())) {
                    int middlePoint = selectedPoint + 1;
                    int otherPoint = selectedPoint + 2;
                    float otherDX = points[otherPoint].x - points[middlePoint].x;
                    float otherDY = points[otherPoint].y - points[middlePoint].y;
                    float otherDistance = std::sqrt( otherDX * otherDX + otherDY * otherDY
                    );
                    Point2D direction =
                        points[middlePoint] - points[selectedPoint];
                    float length = std::sqrt( direction.x * direction.x + direction.y * direction.y);
                    if (length != 0.0f) {
                        direction.x /= length;
                        direction.y /= length;
                        points[otherPoint] = points[middlePoint] + direction * otherDistance;
                    }
                }


                // Moving the control point after a shared endpoint
                else if (selectedPoint % 3 == 1 &&
                        selectedPoint >= 2) {
                    int middlePoint = selectedPoint - 1;
                    int otherPoint = selectedPoint - 2;
                    float otherDX = points[otherPoint].x - points[middlePoint].x;
                    float otherDY = points[otherPoint].y - points[middlePoint].y;
                    float otherDistance = std::sqrt( otherDX * otherDX + otherDY * otherDY);
                    Point2D direction = points[middlePoint] - points[selectedPoint];
                    float length = std::sqrt( direction.x * direction.x + direction.y * direction.y);
                    if (length != 0.0f) {
                        direction.x /= length;
                        direction.y /= length;
                        points[otherPoint] = points[middlePoint] + direction * otherDistance;
                    }
                }
            }
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
        } 
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
            if ((key->code == sf::Keyboard::Key::Equal && key->shift) ||
                key->code == sf::Keyboard::Key::Add) {
                Point2D lastPoint = points[points.size() - 1];
                Point2D previousPoint = points[points.size() - 2];
                Point2D direction = lastPoint - previousPoint;
                float length = std::sqrt( direction.x * direction.x + direction.y * direction.y);
                if (length != 0.0f) {
                    direction.x /= length;
                    direction.y /= length;
                }
                else {
                    direction = {1.0f, 0.0f};
                }
                Point2D newPoint1 = lastPoint + direction * 80.0f;
                Point2D perpendicular = { -direction.y, direction.x};
                Point2D newPoint2 = newPoint1 + direction * 100.0f + perpendicular * 80.0f;
                Point2D newPoint3 = newPoint2 + direction * 100.0f;
                points.push_back(newPoint1);
                points.push_back(newPoint2);
                points.push_back(newPoint3);
            }
            else if (key->code == sf::Keyboard::Key::Hyphen ||
                    key->code == sf::Keyboard::Key::Subtract) {
                if (points.size() > 4) {
                    points.resize(points.size() - 3);
                    if (selectedPoint >= static_cast<int>(points.size())) {
                        selectedPoint = -1;
                    }
                }
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
    const int samples = 200;
    for (std::size_t i = 0; i + 3 < points.size(); i += 3) {
        std::vector<sf::Vector2f> segment = {
            points[i],
            points[i + 1],
            points[i + 2],
            points[i + 3]
        };
        sf::VertexArray curve(sf::PrimitiveType::LineStrip);
        for (int j = 0; j <= samples; j++) {
            float t = static_cast<float>(j) / samples;
            Point2D point = getPoint(segment, t);
            curve.append(sf::Vertex(point, sf::Color::White));
        }
        window.draw(curve);
    }
    // ====== ====== ======
    // TODO: (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======
    int segmentCount = static_cast<int>((points.size() - 1) / 3);
    float fullTime = aniTime * segmentCount;
    int segmentIndex = static_cast<int>(fullTime);
    float localT = fullTime - segmentIndex;
    if (segmentIndex >= segmentCount) {
        segmentIndex = segmentCount - 1;
        localT = 1.0f;
    }
    int start = segmentIndex * 3;
    std::vector<sf::Vector2f> segment = {
        points[start],
        points[start + 1],
        points[start + 2],
        points[start + 3]
    };
    Point2D position = getPoint(segment, localT);
    Point2D slope = getSlope(segment, localT);
    float angle = std::atan2(slope.y, slope.x) * 180.0f / 3.14159265f;
    float size = 20.0f;
    sf::RectangleShape square({size, size});
    square.setOrigin({size / 2.0f,size / 2.0f});
    square.setPosition(position);
    square.setRotation(sf::degrees(angle));
    square.setFillColor(sf::Color::Green);
    window.draw(square);
    aniTime += 0.01f;
    if (aniTime > 1.0f) {
        aniTime = 0.0f;
    }
    // ====== ====== ======
    // TODO: (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    for (std::size_t i = 0; i + 3 < points.size(); i += 3) {
        sf::VertexArray handle1(sf::PrimitiveType::Lines, 2);
        handle1[0].position = points[i];
        handle1[0].color = sf::Color::Yellow;
        handle1[1].position = points[i + 1];
        handle1[1].color = sf::Color::Yellow;
        window.draw(handle1);
        sf::VertexArray handle2(sf::PrimitiveType::Lines, 2);
        handle2[0].position = points[i + 2];
        handle2[0].color = sf::Color::Yellow;
        handle2[1].position = points[i + 3];
        handle2[1].color = sf::Color::Yellow;
        window.draw(handle2);
    }
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======

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
