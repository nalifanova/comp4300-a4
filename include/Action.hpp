#ifndef ACTION_H
#define ACTION_H

#include <string>

#include "Vec2.hpp"


class Action
{
public:
    Action();

    Action(std::string name, std::string type);
    Action(std::string name, Vec2 pos);
    Action(std::string name, std::string type, Vec2 pos);

    [[nodiscard]] const std::string& name() const;
    [[nodiscard]] const std::string& type() const;
    [[nodiscard]] const Vec2& pos() const;
    [[nodiscard]] std::string toString() const;

private:
    std::string m_name;
    std::string m_type;
    Vec2 m_pos = Vec2(0.0f, 0.0f);
};

#endif //ACTION_H
