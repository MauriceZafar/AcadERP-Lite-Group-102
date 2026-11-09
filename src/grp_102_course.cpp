// group-102

#include "grp_102_erp.h"

// Constructor
Course::Course()
{
    code = "";
    type = REGULAR;
    credits = 0;
    category = CORE;
    prereq = -1;
}

// Initialize course information
void Course::setData(const std::string& courseCode,
                     CourseType courseType,
                     int courseCredits,
                     CourseCategory courseCategory,
                     int prerequisite)
{
    code = courseCode;
    type = courseType;
    credits = courseCredits;
    category = courseCategory;
    prereq = prerequisite;
}

// Return course code
const std::string& Course::getCode() const
{
    return code;
}

// Return course type
CourseType Course::getType() const
{
    return type;
}

// Return course credits
int Course::getCredits() const
{
    return credits;
}

// Return course category
CourseCategory Course::getCategory() const
{
    return category;
}

// Return prerequisite index
int Course::getPrerequisite() const
{
    return prereq;
}

// Check whether course is CORE
bool Course::isCore() const
{
    return category == CORE;
}
