
// group-102

#include "grp_102_erp.h"
#include <fstream>

// Check whether a line contains an allowed saved command.
static bool isSavedCommand(const std::string& line)
{
    std::size_t start = line.find_first_not_of(" \t\r");

    if (start == std::string::npos)
        return true;

    if (line[start] == '#')
        return true;

    std::size_t end = line.find_first_of(" \t\r", start);

    std::string command = line.substr(start, end - start);

    return command == "PROGRAM" ||
           command == "COURSE" ||
           command == "STUDENT" ||
           command == "REGISTER" ||
           command == "GRADE" ||
           command == "CONVERT";
}

// Save the complete ERP state.
ErrorCode ERP::saveToFile(const std::string& filename) const
{
    if (filename.empty())
        return E_FILE;

    std::ofstream out(filename.c_str());

    if (!out.is_open())
        return E_FILE;

    // Save program configuration if explicitly defined.
    if (programDefined)
    {
        out << "PROGRAM "
            << program.totalCredits << " "
            << program.electiveCredits << " "
            << program.minGPA / 100 << ".";

        if (program.minGPA % 100 < 10)
            out << "0";

        out << program.minGPA % 100 << " "
            << program.maxLoad << " "
            << program.thesisCredits << "\n";
    }

    // Save all courses.
    for (int i = 0; i < numCourses; i++)
    {
        out << "COURSE "
            << courses[i].getCode() << " ";

        if (courses[i].getType() == REGULAR)
            out << "REGULAR";
        else if (courses[i].getType() == SU)
            out << "SU";
        else
            out << "THESIS";

        out << " " << courses[i].getCredits() << " ";

        if (courses[i].getCategory() == CORE)
            out << "CORE";
        else
            out << "ELECTIVE";

        int prerequisite = courses[i].getPrerequisite();

        if (prerequisite == -1)
            out << " -";
        else
            out << " " << courses[prerequisite].getCode();

        out << "\n";
    }

    // Save all students.
    for (int i = 0; i < numStudents; i++)
    {
        out << "STUDENT "
            << students[i].getRoll() << " "
            << students[i].getName() << "\n";
    }

    // Save registrations and their grades.
    for (int i = 0; i < numStudents; i++)
    {
        const Student& student = students[i];

        for (int j = 0;
             j < student.getRegistrationCount(); j++)
        {
            const Registration* reg =
                student.getRegistration(j);

            int courseIndex = reg->getCourse();

            const Course& course = courses[courseIndex];

            const std::string& roll = student.getRoll();
            const std::string& code = course.getCode();
            int term = reg->getTerm();

            out << "REGISTER "
                << roll << " "
                << code << " "
                << term << "\n";

            const std::string& grade = reg->getGrade();

            if (grade.empty())
                continue;

            // A completed thesis is reconstructed by
            // first restoring S progress and then conversion.
            if (course.getType() == THESIS &&
                reg->getThesisState() == FINAL)
            {
                out << "GRADE "
                    << roll << " "
                    << code << " "
                    << term << " S\n";

                out << "CONVERT "
                    << roll << " "
                    << code << " "
                    << grade << "\n";
            }
            else
            {
                out << "GRADE "
                    << roll << " "
                    << code << " "
                    << term << " "
                    << grade << "\n";
            }
        }
    }

    out.close();

    if (!out)
        return E_FILE;

    return OK;
}

// Load ERP state safely using a temporary ERP object.
ErrorCode ERP::loadFromFile(const std::string& filename)
{
    if (filename.empty())
        return E_FILE;

    std::ifstream in(filename.c_str());

    if (!in.is_open())
        return E_FILE;

    ERP temp;

    std::string line;
    int lineNumber = 0;

    // First reconstruct and validate the entire file.
    while (std::getline(in, line))
    {
        lineNumber++;

        if (!isSavedCommand(line))
            return E_FILE;

        ErrorCode error = temp.processLine(line, lineNumber);

        if (error != OK)
            return E_FILE;
    }

    if (in.bad())
        return E_FILE;

    in.close();

    // Replace courses.
    for (int i = 0; i < MAX_COURSES; i++)
        courses[i] = temp.courses[i];

    // Exchange student ownership safely.
    // The temporary ERP will release the old registrations
    // when it is destroyed.
    for (int i = 0; i < MAX_STUDENTS; i++)
        students[i].swap(temp.students[i]);

    numCourses = temp.numCourses;
    numStudents = temp.numStudents;

    program = temp.program;
    programDefined = temp.programDefined;

    return OK;
}
