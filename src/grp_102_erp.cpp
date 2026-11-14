
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
    commandSeen = false;
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
            return i;
    }

    return -1;
}

// Find student using roll number
int ERP::findStudent(const std::string& roll) const
{
    for (int i = 0; i < numStudents; i++)
    {
        if (students[i].getRoll() == roll)
            return i;
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
        case E_FILE: return "E_FILE";

        default: return "E_PARSE";
    }
}

// Check whether the latest attempt of a course is passed
bool ERP::isCoursePassed(int studentIndex, int courseIndex) const
{
    const Student& student = students[studentIndex];

    int latest = -1;

    for (int i = 0; i < student.getRegistrationCount(); i++)
    {
        const Registration* reg = student.getRegistration(i);

        if (reg != 0 && reg->getCourse() == courseIndex)
            latest = i;
    }

    if (latest == -1)
        return false;

    const Registration* reg = student.getRegistration(latest);
    const Course& course = courses[courseIndex];

    if (course.getType() == SU)
        return reg->getGrade() == "S";

    if (course.getType() == THESIS)
        return reg->getThesisState() == FINAL &&
               gradePoints(reg->getGrade()) >= 4;

    return gradePoints(reg->getGrade()) >= 4;
}

// Calculate earned credits
int ERP::earnedCredits(int studentIndex) const
{
    int total = 0;

    for (int i = 0; i < numCourses; i++)
    {
        if (isCoursePassed(studentIndex, i))
            total += courses[i].getCredits();
    }

    return total;
}

// Perform eligibility checks E2-E6
int ERP::checkEligibility(int studentIndex,
                          int courseIndex,
                          int term) const
{
    const Student& student = students[studentIndex];
    const Course& course = courses[courseIndex];

    // E2: Course must not already be passed
    if (isCoursePassed(studentIndex, courseIndex))
        return 2;

    // E3: No ungraded or active thesis registration
    for (int i = 0; i < student.getRegistrationCount(); i++)
    {
        const Registration* reg = student.getRegistration(i);

        if (reg != 0 && reg->getCourse() == courseIndex)
        {
            if (course.getType() == THESIS)
            {
                if (reg->getThesisState() == REGISTERED ||
                    reg->getThesisState() == IN_PROGRESS)
                    return 3;
            }
            else if (!reg->isGraded())
            {
                return 3;
            }
        }
    }

    // E4: Prerequisite must be passed
    int prerequisite = course.getPrerequisite();

    if (prerequisite != -1 &&
        !isCoursePassed(studentIndex, prerequisite))
        return 4;

    // E5: Term credit limit
    int termCredits = 0;

    for (int i = 0; i < student.getRegistrationCount(); i++)
    {
        const Registration* reg = student.getRegistration(i);

        if (reg != 0 && reg->getTerm() == term)
            termCredits += courses[reg->getCourse()].getCredits();
    }

    if (termCredits + course.getCredits() > program.maxLoad)
        return 5;

    // E6: Thesis eligibility
    if (course.getType() == THESIS &&
        earnedCredits(studentIndex) < program.thesisCredits)
        return 6;

    return 0;
}

// Register a student for a course
ErrorCode ERP::registerCourse(int studentIndex,
                              int courseIndex,
                              int term)
{
    if (studentIndex < 0 || studentIndex >= numStudents)
        return E_UNKNOWN_STUDENT;

    if (courseIndex < 0 || courseIndex >= numCourses)
        return E_UNKNOWN_COURSE;

    int check = checkEligibility(studentIndex, courseIndex, term);

    if (check == 2) return E_ALREADY_PASSED;
    if (check == 3) return E_ALREADY_REGISTERED;
    if (check == 4) return E_PREREQ;
    if (check == 5) return E_CREDIT_LIMIT;
    if (check == 6) return E_THESIS_NOT_ALLOWED;

    ErrorCode result =
        students[studentIndex].addRegistration(courseIndex, term);

    if (result != OK)
        return result;

    int last = students[studentIndex].getRegistrationCount() - 1;

    if (courses[courseIndex].getType() == THESIS)
    {
        students[studentIndex].getRegistration(last)
            ->setThesisState(REGISTERED);
    }

    return OK;
}

// Submit a grade for a registration
ErrorCode ERP::submitGrade(int studentIndex,
                           int courseIndex,
                           int term,
                           const std::string& symbol)
{
    if (studentIndex < 0 || studentIndex >= numStudents)
        return E_UNKNOWN_STUDENT;

    if (courseIndex < 0 || courseIndex >= numCourses)
        return E_UNKNOWN_COURSE;

    Student& student = students[studentIndex];

    int regIndex = student.findRegistration(courseIndex, term);

    if (regIndex == -1)
        return E_NOT_REGISTERED;

    Registration* reg = student.getRegistration(regIndex);
    CourseType type = courses[courseIndex].getType();

    if (type == REGULAR)
    {
        if (gradePoints(symbol) == -1)
        {
            if (symbol == "S" || symbol == "X")
                return E_GRADE_TYPE;

            return E_BAD_GRADE;
        }

        if (reg->isGraded())
            return E_ALREADY_GRADED;

        reg->setGrade(symbol);
        return OK;
    }

    if (type == SU)
    {
        if (symbol != "S" && symbol != "X")
        {
            if (gradePoints(symbol) != -1)
                return E_GRADE_TYPE;

            return E_BAD_GRADE;
        }

        if (reg->isGraded())
            return E_ALREADY_GRADED;

        reg->setGrade(symbol);
        return OK;
    }

    // THESIS
    if (symbol != "S" && symbol != "X")
    {
        if (gradePoints(symbol) != -1)
            return E_GRADE_TYPE;

        return E_BAD_GRADE;
    }

    if (reg->getThesisState() != REGISTERED)
        return E_ALREADY_GRADED;

    reg->setGrade(symbol);

    if (symbol == "S")
        reg->setThesisState(IN_PROGRESS);
    else
        reg->setThesisState(FAILED);

    return OK;
}

// Convert thesis progress into a final letter grade
ErrorCode ERP::convertThesis(int studentIndex,
                             int courseIndex,
                             const std::string& letter)
{
    if (studentIndex < 0 || studentIndex >= numStudents)
        return E_UNKNOWN_STUDENT;

    if (courseIndex < 0 || courseIndex >= numCourses)
        return E_UNKNOWN_COURSE;

    // CONVERT is only allowed for thesis courses
    if (courses[courseIndex].getType() != THESIS)
        return E_GRADE_TYPE;

    if (gradePoints(letter) == -1)
        return E_BAD_GRADE;

    Student& student = students[studentIndex];

    int latest = -1;

    for (int i = 0; i < student.getRegistrationCount(); i++)
    {
        const Registration* reg = student.getRegistration(i);

        if (reg != 0 && reg->getCourse() == courseIndex)
            latest = i;
    }

    if (latest == -1)
        return E_NOT_REGISTERED;

    Registration* reg = student.getRegistration(latest);

    // A thesis already converted to FINAL cannot be converted again.
    if (reg->getThesisState() == FINAL)
        return E_ALREADY_GRADED;

    // REGISTERED or FAILED thesis is not in progress.
    if (reg->getThesisState() != IN_PROGRESS)
        return E_THESIS_STATE;

    reg->setGrade(letter);
    reg->setThesisState(FINAL);

    return OK;
}
