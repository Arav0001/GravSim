#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Util.inl"

struct Particle {
	float mass;
	float radius;

	sf::Vector2f vel;
	sf::Vector2f pos;

	std::string name;
	sf::Color color;

	bool destroyed = false;

	sf::CircleShape obj;

	Particle(const float& mass, const float& radius, const sf::Vector2f& vel, const sf::Vector2f& pos, const std::string& name, const sf::Color& color) {
		this->mass = mass * 1000.0f;
		this->radius = radius;
		this->vel = vel;
		this->pos = pos;
		this->name = name;
		this->color = color;

		this->obj.setPosition(sf::Vector2f(this->pos.x - this->radius, this->pos.y - this->radius));
		this->obj.setFillColor(color);
		this->obj.setRadius(radius);
	}

	void render(sf::RenderWindow& window) {
		if (!destroyed) {
			obj.setPosition(sf::Vector2f(pos.x - radius, pos.y - radius));
			window.draw(obj);
		}
	}

	void updatePhysics(const Particle& p) {
		if (destroyed) {
			return;
		}

		if (!p.destroyed) {
			const float g = 6.6743f * (float) pow(10, -11);

			auto dir = p.pos - pos;
			util::setMag(dir, 0.0005f * util::getMag(dir));
			const auto magSq = dir.x * dir.x + dir.y * dir.y;

			const float force = g * mass * p.mass / magSq;

			dir *= force * util::Q_rsqrt(magSq);
			vel += dir / mass;
		}

		pos += vel;
	}

	void drawStatistics(const float x, const float y, const sf::Color c, sf::RenderWindow& window) const {
		const std::string statistics = name + ": vel = " + std::to_string(vel.x * 60.0f) + " pixels/s, " + std::to_string(vel.y * 60.0f) + " pixels/s, pos = " + std::to_string(pos.x) + ", " + std::to_string(pos.y);
		util::drawText(statistics, 14, x, y, c, window);
	}

	void drawArrows(sf::RenderWindow& window, const sf::Color c) const {
		util::drawLine(pos, pos + vel * 60.0f, c, window);
		sf::VertexArray tip(sf::Triangles, 3);

		sf::Vector2f base_vec(vel.x, vel.y);
		util::setMag(base_vec, 7.5f);

		sf::Transform rotation135;
		rotation135.rotate(135);

		sf::Transform rotation225;
		rotation225.rotate(225);

		tip.append(sf::Vertex(pos + vel * 60.0f + vel, c));
		tip.append(sf::Vertex(pos + vel * 60.0f + rotation135.transformPoint(base_vec), c));
		tip.append(sf::Vertex(pos + vel * 60.0f + rotation225.transformPoint(base_vec), c));

		window.draw(tip);
	}

	void drawLabels(sf::RenderWindow& window, const sf::Color c) const {
		if (!destroyed) util::drawText(name, 14, obj.getPosition().x, obj.getPosition().y - 10.0f - radius, c, window);
	}

	void destroy() {
		destroyed = true;
		pos = { 0.0f, 0.0f };
		vel = { 0.0f, 0.0f };
	}

	void checkCollisions(Particle& p) {
		if (obj.getGlobalBounds().intersects(p.obj.getGlobalBounds())) {
			if (radius > p.radius) {
				p.destroy();
			}
			else if (radius < p.radius) {
				destroy();
			}
			else {
				destroy();
				p.destroy();
			}
		}
	}
};

class ParticleSystem {
public:
	ParticleSystem() = default;

	std::vector<Particle> particles;

	void addParticle(const Particle &p) {
		particles.push_back(p);
	}

	void render(sf::RenderWindow &window) {
		for (auto& particle : particles) {
			particle.render(window);
		}
	}

	void drawStatistics(const sf::Vector2f offset, sf::RenderWindow& window, const sf::Color color, const bool showStatistics) const {
		if (showStatistics) {
			for (int i = 0; i < (int) particles.size(); i++) {
				particles[i].drawStatistics(offset.x + 10.0f, 20.0f * static_cast<float>(i) + offset.y + 10.0f, color, window);
			}
		}
	}

	void drawLabels(sf::RenderWindow& window, const sf::Color color, const bool showLabels) const {
		const sf::FloatRect mouseBounds(sf::Vector2f(sf::Mouse::getPosition()).x - 7.5f, sf::Vector2f(sf::Mouse::getPosition()).y - 7.5f, 15.0f, 15.0f);
		for (const auto& particle : particles) {
			if (showLabels || particle.obj.getGlobalBounds().intersects(mouseBounds) || particle.obj.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition()))) {
				particle.drawLabels(window, color);
			}
		}
	}

	void drawArrows(sf::RenderWindow& window, const sf::Color color, const bool showArrows) const {
		if (showArrows) {
			for (const auto& particle : particles) {
				particle.drawArrows(window, color);
			}
		}
	}

	void updatePhysics(const bool compute, const bool paused) {
		if (particles.empty() || paused) return;
		
		if (particles.size() == 1) {
			particles[0].pos += particles[0].vel;
		} else {
			for (int i = 0; i < (int) particles.size(); i++) {
				for (int j = 0; j < (int) particles.size(); j++) {
					if (!(i == j)) {
						particles[i].checkCollisions(particles[j]);

						if (compute) {
							particles[i].updatePhysics(particles[j]);
						} else {
							particles[i].pos += particles[i].vel;
							particles[j].pos += particles[j].vel;
						}
					}
				}
			}
		}
	}

	void cleanUp() {
		for (int i = 0; i < (int) particles.size(); i++) {
			if (particles[i].destroyed) particles.erase(particles.begin() + i);
		}
	}
};

#endif
