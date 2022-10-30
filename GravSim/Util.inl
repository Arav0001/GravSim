#ifndef UTIL_INL
#define UTIL_INL

#include <SFML/Graphics.hpp>
#include "consolas_font.h"

namespace util {
	const std::string HELP_TEXT = "Welcome to GravSim!\n\nHow To Use\n\n  1. After launching you should see a grid on your screen.\n  2. To maneuver around the grid you can use WASD.\n  3. To create a particle click with your left mouse button.\n  4. Once you have clicked, drag your mouse to a position to make the radius of your particle.\n  5. Then use the Up and Down arrow keys to increase or decrease the mass of the particle.\n  6. Once you have modified all the parameters of your particle you can click ENTER to finalize the creation.\n  7. If you ever want to pause the simulation you can just click SPACE at any time (except during particle creation)\n     to pause the simulation.\n  8. Make more particles and have fun experiencing gravity in real-time!\n\nCONTROLS\n\nW - Move view UP\nA - Move view LEFT\nS - Move view DOWN\nD - Move view RIGHT\n\nLEFT CLICK - Create particle\nENTER - Confirm create particle\nUP ARROW - Increase mass\nDOWN ARROW - Decrease mass\n\nESCAPE - Quit application\n\nF1 - Toggle statistics\nF2 - Toggle arrows\nF3 - Toggle labels\nF4 - Toggle help menu";

	inline void drawText(const std::string& str, const int size, const float& x, const float& y, const sf::Color color, sf::RenderWindow& window) {
		auto font = sf::Font();
		font.loadFromMemory(&Consolas_ttf, Consolas_ttf_len);
		sf::Text text(str, font, size);
		text.setFillColor(color);
		text.setPosition(x, y);

		window.draw(text);
	}

	// credits to the Quake III developers for this amazing algorithm!
	inline float Q_rsqrt(float number) {
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;
		i = 0x5f3759df - (i >> 1);
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));
		y = y * (threehalfs - (x2 * y * y));

		return y;
	}

	inline float getMag(const sf::Vector2f& vec) {
		return 1 / Q_rsqrt(vec.x * vec.x + vec.y * vec.y);
	}

	inline void setMag(sf::Vector2f& vec, const float& mag) {
		vec *= mag * Q_rsqrt(vec.x * vec.x + vec.y * vec.y);
	}

	inline void drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Color& color, sf::RenderWindow& window) {
		sf::VertexArray line(sf::Lines, 2);
		line.append(sf::Vertex(p1, color));
		line.append(sf::Vertex(p2, color));

		window.draw(line);
	}

	inline void drawDottedLine(const sf::Vector2i& p1, const sf::Vector2i& p2, const sf::Color& color, sf::RenderWindow& window) {
		sf::VertexArray line(sf::Lines, 2);
		line.append(sf::Vertex(sf::Vector2f(p1), color));
		line.append(sf::Vertex(sf::Vector2f(p2), color));

		window.draw(line);
	}

	inline void drawGrid(sf::RenderWindow& window, const sf::Vector2i& vecOffset, const sf::Color& color) {
		const auto offset = vecOffset - sf::Vector2i(vecOffset.x % 40, vecOffset.y % 40);

		for (int x = 0; x < (int)window.getSize().x / 40 + 1; x++) {
			drawDottedLine(sf::Vector2i(40 * x, -40) + offset, sf::Vector2i(40 * x, (int)window.getSize().y + 40) + offset, color, window);
		}

		for (int y = 0; y < (int)window.getSize().y / 40 + 1; y++) {
			drawDottedLine(sf::Vector2i(-40, 40 * y) + offset, sf::Vector2i((int)window.getSize().x + 40, 40 * y) + offset, color, window);
		}
	}
}

#endif
