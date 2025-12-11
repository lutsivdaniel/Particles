#include "Particle.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;
using namespace Matrices;

// Constructor
Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
    : m_ttl(TTL),
    m_numPoints(numPoints),
    m_centerCoordinate(0.0f, 0.0f),
    m_radiansPerSec(0.0f),
    m_vx(0.0f),
    m_vy(0.0f),
    m_cartesianPlane(),
    m_color1(Color::White),
    m_color2(),
    m_A(2, numPoints)
{
    // Set up the Cartesian view:
    float width = static_cast<float>(target.getSize().x);
    float height = static_cast<float>(target.getSize().y);

    m_cartesianPlane.setCenter(0.0f, 0.0f);
    m_cartesianPlane.setSize(width, -height);

    // Map mouse click (pixel coords) to Cartesian coords using this view
    Vector2f centerCoords = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);
    m_centerCoordinate = centerCoords;

    // Random angular velocity in [0, PI]
    float fraction = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    m_radiansPerSec = fraction * static_cast<float>(M_PI);

    // Random initial horizontal speed in [100, 500], random sign
    float baseVx = 100.0f + static_cast<float>(rand() % 401);
    if (rand() % 2 != 0)
    {
        baseVx = -baseVx;
    }
    m_vx = baseVx;

    // Random initial vertical speed in [100, 500] (upward in Cartesian y)
    m_vy = 100.0f + static_cast<float>(rand() % 401);

    // Colors
    m_color1 = Color::White;
    m_color2 = Color(
        static_cast<Uint8>(rand() % 256),
        static_cast<Uint8>(rand() % 256),
        static_cast<Uint8>(rand() % 256)
    );

    // Generate numPoints vertices by sweeping an arc with random radii
    float theta = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
        * static_cast<float>(M_PI / 2.0);
    float dTheta = static_cast<float>(2.0 * M_PI) / static_cast<float>(numPoints - 1);

    for (int j = 0; j < numPoints; j++)
    {
        // Random radius in [20, 80]
        float r = 20.0f + static_cast<float>(rand() % 61); // 20 to 80

        float dx = r * cos(theta);
        float dy = r * sin(theta);

        m_A(0, j) = static_cast<double>(m_centerCoordinate.x + dx);
        m_A(1, j) = static_cast<double>(m_centerCoordinate.y + dy);

        theta += dTheta;
    }
}

// Draw particle as a triangle fan
void Particle::draw(RenderTarget& target, RenderStates states) const
{
    // TriangleFan: center + outer ring of points
    VertexArray lines(TriangleFan, m_numPoints + 1);

    // Map center from Cartesian coords back to pixel coords
    Vector2i centerPixel = target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane);
    Vector2f center(static_cast<float>(centerPixel.x), static_cast<float>(centerPixel.y));

    // Center vertex
    lines[0].position = center;
    lines[0].color = m_color1;

    // Outer vertices
    for (int j = 1; j <= m_numPoints; j++)
    {
        int col = j - 1;

        Vector2f cartCoord(
            static_cast<float>(m_A(0, col)),
            static_cast<float>(m_A(1, col))
        );

        Vector2i pixel = target.mapCoordsToPixel(cartCoord, m_cartesianPlane);

        lines[j].position = Vector2f(
            static_cast<float>(pixel.x),
            static_cast<float>(pixel.y)
        );

        lines[j].color = m_color2;
    }

    target.draw(lines, states);
}

// Update particle each frame
void Particle::update(float dt)
{
    m_ttl -= dt;
    rotate(static_cast<double>(dt * m_radiansPerSec));
    scale(static_cast<double>(SCALE));
    float dx = m_vx * dt;
    m_vy -= G * dt;
    float dy = m_vy * dt;
    translate(static_cast<double>(dx), static_cast<double>(dy));
}

// Rotate around the particle center
void Particle::rotate(double theta)
{
    Vector2f temp = m_centerCoordinate;
    translate(-static_cast<double>(m_centerCoordinate.x),
        -static_cast<double>(m_centerCoordinate.y));

    RotationMatrix R(theta);
    m_A = R * m_A;

    translate(static_cast<double>(temp.x), static_cast<double>(temp.y));
}

// Scale relative to the particle center
void Particle::scale(double c)
{
    Vector2f temp = m_centerCoordinate;
    translate(-static_cast<double>(m_centerCoordinate.x),
        -static_cast<double>(m_centerCoordinate.y));

    ScalingMatrix S(c);
    m_A = S * m_A;

    translate(static_cast<double>(temp.x), static_cast<double>(temp.y));
}

// Translate by (xShift, yShift)
void Particle::translate(double xShift, double yShift)
{
    int nCols = m_A.getCols();
    TranslationMatrix T(xShift, yShift, nCols);

    m_A = T + m_A;

    m_centerCoordinate.x += static_cast<float>(xShift);
    m_centerCoordinate.y += static_cast<float>(yShift);
}

bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed. Expected (0,0). Received: (" << m_centerCoordinate.x <<
            "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed. +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1,
            j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
                rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) || !
            almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
                scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !
            almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
                translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed. +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}