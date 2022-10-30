#include <chrono>

#include "ParticleSystem.h"

class GravSim {
public:
    GravSim() = default;

    sf::RenderWindow window;
    sf::View view;

    ParticleSystem particleSystem;

    bool paused = false, showStatistics = true, showArrows = true, showLabels = true;

    long long ticksElapsed = 0;
    bool doCompute = true;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    float fps = 0;

    sf::Vector2f viewOffset;

    sf::CircleShape tempCircle;

    void run() {
        window.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "GravSim", sf::Style::Fullscreen);
        window.setVerticalSyncEnabled(true);

        view = sf::View(sf::FloatRect(0, 0, (float)sf::VideoMode::getDesktopMode().width, (float)sf::VideoMode::getDesktopMode().height));
        window.setView(view);

        tempCircle.setFillColor(sf::Color(0, 0, 0, 0));

        while (window.isOpen()) {
            loop();
        }
    }

    void loop() {
        handleEvents();

        viewOffset = view.getCenter() - view.getSize() / 2.0f;
        doCompute = ticksElapsed % 6 == 0;

        start = std::chrono::high_resolution_clock::now();

        window.setView(view);
        window.clear(sf::Color::Black);

        util::drawGrid(window, sf::Vector2i(viewOffset), sf::Color(255, 255, 255, 64));
        util::drawText(std::to_string(ticksElapsed / 216000) + "h " + std::to_string(ticksElapsed / 3600) + "m " + std::to_string(ticksElapsed / 60) + "s elapsed", 16.0f, 10.0f + viewOffset.x, (float)window.getSize().y - 26.0f + viewOffset.y, sf::Color::White, window);
        util::drawText(std::to_string((int)round(fps)) + " FPS", 16.0f, (float)window.getSize().x - 70.0f + viewOffset.x, 10.0f + viewOffset.y, sf::Color::White, window);

        particleSystem.render(window);
        particleSystem.updatePhysics(doCompute, paused);

        particleSystem.drawArrows(window, sf::Color::Red, showArrows);
        particleSystem.drawLabels(window, sf::Color::Cyan, showLabels);
        particleSystem.drawStatistics(viewOffset, window, sf::Color::White, showStatistics);

        window.display();

        end = std::chrono::high_resolution_clock::now();
        fps = (float)1e9 / (float)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        particleSystem.cleanUp();

        if (!paused) ticksElapsed++;
    }

    void handleEvents() {
        sf::Event event = {};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape) window.close();

            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) paused = !paused;

            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F1) showStatistics = !showStatistics;
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F2) showArrows = !showArrows;
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F3) showLabels = !showLabels;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) view.move(sf::Vector2f(+0.0f, -5.0f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) view.move(sf::Vector2f(+0.0f, +5.0f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) view.move(sf::Vector2f(-5.0f, +0.0f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) view.move(sf::Vector2f(+5.0f, +0.0f));

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            particleSystem.addParticle(handleParticle(event));
        }
    }

    Particle handleParticle(sf::Event event) {
        paused = true;

    	tempCircle.setFillColor(sf::Color::White);
    	const auto initPos = sf::Vector2f(sf::Mouse::getPosition()) + viewOffset;
    	float radius = 0;
        float mass = 0;
        const std::string name = "p" + std::to_string((int)particleSystem.particles.size());

        while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            window.pollEvent(event);

        	const auto finalPos = sf::Vector2f(sf::Mouse::getPosition()) + viewOffset;
            const auto mouseOffset = finalPos - initPos;
            radius = 1 / util::Q_rsqrt(mouseOffset.x * mouseOffset.x + mouseOffset.y * mouseOffset.y);
            tempCircle.setPosition(sf::Vector2f(initPos.x - radius, initPos.y - radius));
            tempCircle.setRadius(radius);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                if (mass + 1 < FLT_MAX) mass += 1;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                if (mass - 1 >= 0) mass -= 1;
            }

        	window.clear(sf::Color::Black);

            util::drawGrid(window, sf::Vector2i(viewOffset), sf::Color(255, 255, 255, 64));
            window.draw(tempCircle);
            util::drawText("mass = " + std::to_string(mass), 14, initPos.x + radius + 5.0f, initPos.y, sf::Color::Cyan, window);

            window.display();
        }

        Particle p(mass, radius, sf::Vector2f(0.0f, 0.0f), initPos, name, sf::Color::White);

        paused = false;

        return p;
    }
};

int main() {
    GravSim gravSim;
    gravSim.run();

    return 0;
}