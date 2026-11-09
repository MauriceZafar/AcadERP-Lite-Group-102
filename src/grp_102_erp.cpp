// group-102

#include "grp_102_erp.h"

// ERP constructor
ERP::ERP()
{
    numCourses = 0;
    numStudents = 0;

    program.totalCredits = 160;
    program.electiveCredits = 24;
    program.minGPA = 500;
    program.maxLoad = 24;
    program.thesisCredits = 100;

    programDefined = false;
}

// ERP destructor
ERP::~ERP()
{
}

// Find course using course code
int ERP::findCourse(const std::string& code) const
{
    for (int i = 0; i < numCourses; i++)
    {
        if (courses[i].getCode() == code)
        {
            return i;
        }
    }

    return -1;
}

// Find student using roll number
int ERP::findStudent(const std::string& roll) const
{
    for (int i = 0; i < numStudents; i++)
    {
        if (students[i].getRoll() == roll)
        {
            return i;
        }
    }

    return -1;
}

// Convert letter grade into grade points
int ERP::gradePoints(const std::string& symbol) const
{
    if (symbol == "A")  return 10;
    if (symbol == "A-") return 9;
    if (symbol == "B")  return 8;
    if (symbol == "B-") return 7;
    if (symbol == "C")  return 6;
    if (symbol == "C-") return 5;
    if (symbol == "D")  return 4;
    if (symbol == "F")  return 0;

    return -1;
}

// Convert error code into error name
const char* ERP::errorName(ErrorCode error) const
{
    switch (error)
    {
        case OK: return "OK";
        case E_PARSE: return "E_PARSE";
        case E_FULL: return "E_FULL";
        case E_DUPLICATE: return "E_DUPLICATE";
        case E_UNKNOWN_STUDENT: return "E_UNKNOWN_STUDENT";
        case E_UNKNOWN_COURSE: return "E_UNKNOWN_COURSE";
        case E_BAD_COURSE: return "E_BAD_COURSE";
        case E_BAD_GRADE: return "E_BAD_GRADE";
        case E_GRADE_TYPE: return "E_GRADE_TYPE";
        case E_NOT_REGISTERED: return "E_NOT_REGISTERED";
        case E_ALREADY_GRADED: return "E_ALREADY_GRADED";
        case E_THESIS_STATE: return "E_THESIS_STATE";
        case E_ALREADY_PASSED: return "E_ALREADY_PASSED";
        case E_ALREADY_REGISTERED: return "E_ALREADY_REGISTERED";
        case E_PREREQ: return "E_PREREQ";
        case E_CREDIT_LIMIT: return "E_CREDIT_LIMIT";
        case E_THESIS_NOT_ALLOWED: return "E_THESIS_NOT_ALLOWED";
        case E_NO_GPA: return "E_NO_GPA";
        default: return "E_PARSE";
    }
}
