#ifndef UTIL_INL
#define UTIL_INL

#include <SFML/Graphics.hpp>
#include "consolas_font.h"

namespace util {
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
