// group-102
// group-102

#include "grp_102_erp.h"

// Constructor
Registration::Registration()
{
    course = -1;
    term = 0;
    grade = "";
    thesis = NOT_THESIS;
}

// Initialize registration
void Registration::initialize(int courseIndex, int semester)
{
    course = courseIndex;
    term = semester;
    grade = "";
    thesis = NOT_THESIS;
}

// Return course index
int Registration::getCourse() const
{
    return course;
}

// Return semester
int Registration::getTerm() const
{
    return term;
}

// Return grade
const std::string& Registration::getGrade() const
{
    return grade;
}

// Return thesis state
ThesisState Registration::getThesisState() const
{
    return thesis;
}

// Store grade
void Registration::setGrade(const std::string& symbol)
{
    grade = symbol;
}

// Update thesis state
void Registration::setThesisState(ThesisState state)
{
    thesis = state;
}

// Check whether registration has a grade
bool Registration::isGraded() const
{
    return !grade.empty();
}
