#include <iostream>
#include <string>
class Formatter {
    public:
        static std::string formattingMoney(int money) {
            return std::to_string(money);
        }

        static std::string formatTileNameAndCode(std::string tileName, std::string tileCode) {
            return tileName + " " + tileCode;
        }

        static std::string formatLog(int turn, std::string username, std::string actionType, std::string detail) {
            return std::to_string(turn) + " " + username + " " + actionType + " " + detail + "\n";
        }
};