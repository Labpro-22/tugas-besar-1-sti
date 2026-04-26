#pragma once

#include <string>

class Player;

class SkillCard {
	protected:
		std::string name;
		std::string description;

	public:
		SkillCard(std::string name, std::string desc) : name(name), description(desc) {}
		virtual ~SkillCard() = default;

		// Abstract method
		virtual void activate(Player& owner) = 0;

		// Getter
		std::string getName() const { return name; }
		std::string getDescription() const { return description; }
};
