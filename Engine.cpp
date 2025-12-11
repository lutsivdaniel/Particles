#include "Engine.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;
using namespace sf;

Engine::Engine()
{
    VideoMode vm = VideoMode::getDesktopMode();
    m_Window.create(vm, "Particle Fireworks", Style::Default);

    srand(static_cast<unsigned int>(time(nullptr)));
}

void Engine::run()
{
    // Unit tests first
    cout << "Starting Particle unit tests..." << endl;
    Particle p(
        m_Window,
        4,
        Vector2i(
            static_cast<int>(m_Window.getSize().x) / 2,
            static_cast<int>(m_Window.getSize().y) / 2
        )
    );
    p.unitTests();
    cout << "Unit tests complete. Starting engine..." << endl;

    Clock clock;

    while (m_Window.isOpen())
    {
        Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();

        input();
        update(dtAsSeconds);
        draw();
    }
}

void Engine::input()
{
    Event event;

    while (m_Window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            m_Window.close();
        }
        else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
        {
            m_Window.close();
        }
        else if (event.type == Event::MouseButtonPressed &&
            event.mouseButton.button == Mouse::Left)
        {
            Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);

            // Create particles per click
            int numParticlesPerClick = 5;

            for (int i = 0; i < numParticlesPerClick; i++)
            {
                // Random numPoints in [25, 50]
                int numPoints = 25 + (rand() % 26);

                m_particles.emplace_back(m_Window, numPoints, clickPos);
            }
        }
    }
}

void Engine::update(float dtAsSeconds)
{
    // Iterate through particles and update or erase
    auto it = m_particles.begin();

    while (it != m_particles.end())
    {
        if (it->getTTL() > 0.0f)
        {
            it->update(dtAsSeconds);
            ++it;
        }
        else
        {
            it = m_particles.erase(it);
        }
    }
}

void Engine::draw()
{
    m_Window.clear();

    for (size_t i = 0; i < m_particles.size(); i++)
    {
        m_Window.draw(m_particles[i]);
    }

    m_Window.display();
}