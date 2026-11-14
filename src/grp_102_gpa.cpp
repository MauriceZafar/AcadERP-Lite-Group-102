
// group-102

#include "grp_102_erp.h"

// Calculate GPA in hundredths using integer arithmetic
ErrorCode ERP::gpaHundredths(int studentIndex, int* result) const
{
    if (studentIndex < 0 || studentIndex >= numStudents ||
        result == 0)
        return E_NO_GPA;

    const Student& student = students[studentIndex];

    long long numerator = 0;
    long long denominator = 0;

    for (int courseIndex = 0; courseIndex < numCourses; courseIndex++)
    {
        int latest = -1;

        // Find the latest graded attempt by registration order
        for (int i = 0; i < student.getRegistrationCount(); i++)
        {
            const Registration* reg = student.getRegistration(i);

            if (reg != 0 &&
                reg->getCourse() == courseIndex &&
                reg->isGraded())
            {
                latest = i;
            }
        }

        if (latest == -1)
            continue;

        const Registration* reg = student.getRegistration(latest);
        const Course& course = courses[courseIndex];

        // SU courses do not contribute to GPA
        if (course.getType() == SU)
            continue;

        // Thesis contributes only after FINAL conversion
        if (course.getType() == THESIS &&
            reg->getThesisState() != FINAL)
            continue;

        int points = gradePoints(reg->getGrade());

        if (points < 0)
            continue;

        numerator +=
            static_cast<long long>(course.getCredits()) * points;

        denominator += course.getCredits();
    }

    if (denominator == 0)
        return E_NO_GPA;

    // Round half-up to two decimal places
    *result = static_cast<int>(
        (200 * numerator + denominator) / (2 * denominator)
    );

    return OK;
}

// Calculate average grade from rounded GPA
ErrorCode ERP::getAverageGrade(int studentIndex,
                               std::string* result) const
{
    if (result == 0)
        return E_NO_GPA;

    int h = 0;

    ErrorCode error = gpaHundredths(studentIndex, &h);

    if (error != OK)
        return error;

    if (h >= 950) *result = "A";
    else if (h >= 850) *result = "A-";
    else if (h >= 750) *result = "B";
    else if (h >= 650) *result = "B-";
    else if (h >= 550) *result = "C";
    else if (h >= 450) *result = "C-";
    else if (h >= 200) *result = "D";
    else *result = "F";

    return OK;
}

// Calculate graduation status and list every failed rule
ErrorCode ERP::getGraduationStatus(int studentIndex,
                                   std::string* result) const
{
    if (studentIndex < 0 || studentIndex >= numStudents ||
        result == 0)
        return E_UNKNOWN_STUDENT;

    const Student& student = students[studentIndex];

    // R1: Total earned credits
    bool failR1 =
        earnedCredits(studentIndex) < program.totalCredits;

    // R2: Every CORE non-thesis course must be passed
    bool failR2 = false;

    // R3: Passed elective credits
    int electiveCredits = 0;

    // R4: Thesis must have a latest graded FINAL letter of C or better
    bool thesisFound = false;
    bool failR4 = false;

    for (int i = 0; i < numCourses; i++)
    {
        const Course& course = courses[i];

        bool passed = isCoursePassed(studentIndex, i);

        if (course.isCore() &&
            course.getType() != THESIS &&
            !passed)
        {
            failR2 = true;
        }

        if (!course.isCore() && passed)
        {
            electiveCredits += course.getCredits();
        }

        if (course.getType() == THESIS)
        {
            thesisFound = true;

            int latest = -1;

            // Find the latest graded thesis attempt
            for (int j = 0; j < student.getRegistrationCount(); j++)
            {
                const Registration* reg =
                    student.getRegistration(j);

                if (reg != 0 &&
                    reg->getCourse() == i &&
                    reg->isGraded())
                {
                    latest = j;
                }
            }

            bool thesisPassed = false;

            if (latest != -1)
            {
                const Registration* reg =
                    student.getRegistration(latest);

                if (reg->getThesisState() == FINAL &&
                    gradePoints(reg->getGrade()) >= 6)
                {
                    thesisPassed = true;
                }
            }

            if (!thesisPassed)
                failR4 = true;
        }
    }

    // No thesis course in catalogue also fails R4
    if (!thesisFound)
        failR4 = true;

    // R3: Check elective credit requirement
    bool failR3 =
        electiveCredits < program.electiveCredits;

    // R5: GPA must meet minimum requirement
    int gpa = 0;

    ErrorCode gpaError =
        gpaHundredths(studentIndex, &gpa);

    bool failR5 =
        (gpaError != OK || gpa < program.minGPA);

    // Build result in required R1-R5 order
    *result = "";

    if (failR1) *result += "R1,";
    if (failR2) *result += "R2,";
    if (failR3) *result += "R3,";
    if (failR4) *result += "R4,";
    if (failR5) *result += "R5,";

    if (result->empty())
    {
        *result = "YES";
    }
    else
    {
        *result =
            "NO " + result->substr(0, result->length() - 1);
    }

    return OK;
}
