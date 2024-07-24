#include "Action.h"
#include <string>
const std::string& Action::name() const
{
    return m_name;
}

const std::string& Action::type() const
{
    return m_type;
}
