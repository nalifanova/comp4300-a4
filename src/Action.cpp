#include "Action.hpp"

#include <sstream>

Action::Action() = default;

Action::Action(std::string name, std::string type) :
    m_name(std::move(name)), m_type(std::move(type)) {}

Action::Action(std::string name, Vec2 pos) :
    m_name(std::move(name)), m_pos(pos) {}

Action::Action(std::string name, std::string type, Vec2 pos) :
    m_name(std::move(name)), m_type(std::move(type)), m_pos(pos) {}

const std::string& Action::name() const
{
    return m_name;
}

const std::string& Action::type() const
{
    return m_type;
}

const Vec2& Action::pos() const
{
    return m_pos;
}

std::string Action::toString() const
{
    std::stringstream ss;
    ss << name() << " " << type() << " "
        << static_cast<int>(pos().x) << " "
        << static_cast<int>(pos().y);
    return ss.str();
}
