// group-102

#include "grp_102_erp.h"

static int splitWords(const std::string& line,
                      std::string words[], int capacity)
{
    int count = 0;
    int i = 0;

    while (i < static_cast<int>(line.length()))
    {
        while (i < static_cast<int>(line.length()) &&
               (line[i] == ' ' || line[i] == '\t' ||
                line[i] == '\r'))
            i++;

        if (i >= static_cast<int>(line.length()))
            break;

        if (count >= capacity)
            return -1;

        std::string word = "";

        while (i < static_cast<int>(line.length()) &&
               line[i] != ' ' && line[i] != '\t' &&
               line[i] != '\r')
        {
            word += line[i++];
        }

        words[count++] = word;
    }

    return count;
}

static bool parseInteger(const std::string& value, int* result)
{
    if (value.empty() || result == 0)
        return false;

    int number = 0;

    for (int i = 0; i < static_cast<int>(value.length()); i++)
    {
        if (value[i] < '0' || value[i] > '9')
            return false;

        int digit = value[i] - '0';

        if (number > (2147483647 - digit) / 10)
            return false;

        number = number * 10 + digit;
    }

    *result = number;
    return true;
}

static bool parseGPA(const std::string& value, int* result)
{
    std::size_t dot = value.find('.');

    if (dot == std::string::npos || dot == 0 ||
        dot != value.length() - 3 || result == 0)
        return false;

    int whole = 0;
    int fraction = 0;

    if (!parseInteger(value.substr(0, dot), &whole) ||
        !parseInteger(value.substr(dot + 1, 2), &fraction))
        return false;

    if (whole > 10 || fraction > 99 ||
        (whole == 10 && fraction != 0))
        return false;

    *result = whole * 100 + fraction;
    return true;
}

ErrorCode ERP::processLine(const std::string& line, int lineNumber)
{
    (void)lineNumber;

    // FIX 1: Ignore blank lines and comments BEFORE tokenizing.
    // This also handles comments containing more than 7 words.
    std::size_t first = line.find_first_not_of(" \t\r");

    if (first == std::string::npos)
        return OK;

    if (line[first] == '#')
        return OK;

    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count == 0)
        return OK;

    if (count < 0)
    {
        commandSeen = true;
        return E_PARSE;
    }

    // PROGRAM must be the first non-blank, non-comment command.
    bool isFirstCommand = !commandSeen;
    commandSeen = true;

    if (words[0] == "PROGRAM")
    {
        if (!isFirstCommand || count != 6)
            return E_PARSE;

        return setProgram(line);
    }

    if (words[0] == "COURSE")
    {
        if (count != 6) return E_PARSE;
        return addCourse(line);
    }

    if (words[0] == "STUDENT")
    {
        if (count != 3) return E_PARSE;
        return addStudent(line);
    }

    if (words[0] == "REGISTER")
    {
        if (count != 4) return E_PARSE;

        int term = 0;

        if (!parseInteger(words[3], &term) ||
            term < 1 || term > 12)
            return E_PARSE;

        int studentIndex = findStudent(words[1]);

        if (studentIndex == -1)
            return E_UNKNOWN_STUDENT;

        int courseIndex = findCourse(words[2]);

        if (courseIndex == -1)
            return E_UNKNOWN_COURSE;

        return registerCourse(studentIndex, courseIndex, term);
    }

    if (words[0] == "GRADE")
    {
        if (count != 5) return E_PARSE;

        int term = 0;

        if (!parseInteger(words[3], &term) ||
            term < 1 || term > 12)
            return E_PARSE;

        int studentIndex = findStudent(words[1]);

        if (studentIndex == -1)
            return E_UNKNOWN_STUDENT;

        int courseIndex = findCourse(words[2]);

        if (courseIndex == -1)
            return E_UNKNOWN_COURSE;

        return submitGrade(studentIndex, courseIndex,
                           term, words[4]);
    }

    if (words[0] == "CONVERT")
    {
        if (count != 4) return E_PARSE;

        int studentIndex = findStudent(words[1]);

        if (studentIndex == -1)
            return E_UNKNOWN_STUDENT;

        int courseIndex = findCourse(words[2]);

        if (courseIndex == -1)
            return E_UNKNOWN_COURSE;

        return convertThesis(studentIndex, courseIndex,
                             words[3]);
    }

    // TRANSCRIPT command
    if (words[0] == "TRANSCRIPT")
    {
        if (count != 2) return E_PARSE;

        int studentIndex = findStudent(words[1]);

        if (studentIndex == -1)
            return E_UNKNOWN_STUDENT;

        const Student& student = students[studentIndex];

        std::cout << "TRANSCRIPT " << words[1] << "\n";

        for (int term = 1; term <= 12; term++)
        {
            bool termFound = false;

            for (int i = 0;
                 i < student.getRegistrationCount(); i++)
            {
                const Registration* reg =
                    student.getRegistration(i);

                if (reg->getTerm() == term)
                {
                    if (!termFound)
                    {
                        std::cout << "TERM "
                                  << term << "\n";
                        termFound = true;
                    }

                    int courseIndex = reg->getCourse();

                    const Course& course =
                        courses[courseIndex];

                    std::string type;

                    if (course.getType() == REGULAR)
                        type = "REGULAR";
                    else if (course.getType() == SU)
                        type = "SU";
                    else
                        type = "THESIS";

                    std::string grade = reg->getGrade();

                    if (grade.empty())
                        grade = "-";

                    std::cout << course.getCode() << " "
                              << type << " "
                              << course.getCredits() << " "
                              << grade << "\n";
                }
            }
        }

        return OK;
    }

    // SAVE command
    if (words[0] == "SAVE")
    {
        if (count != 2) return E_PARSE;

        return saveToFile(words[1]);
    }

    // LOAD command
    if (words[0] == "LOAD")
    {
        if (count != 2) return E_PARSE;

        return loadFromFile(words[1]);
    }

    if (words[0] == "GPA")
    {
        if (count != 2) return E_PARSE;

        int studentIndex = findStudent(words[1]);
        if (studentIndex == -1) return E_UNKNOWN_STUDENT;

        int value = 0;
        ErrorCode error = gpaHundredths(studentIndex, &value);

        if (error != OK) return error;

        std::cout << "GPA " << words[1] << " "
                  << value / 100 << ".";

        if (value % 100 < 10)
            std::cout << "0";

        std::cout << value % 100 << "\n";
        return OK;
    }

    if (words[0] == "AVG")
    {
        if (count != 2) return E_PARSE;

        int studentIndex = findStudent(words[1]);
        if (studentIndex == -1) return E_UNKNOWN_STUDENT;

        std::string average;
        ErrorCode error = getAverageGrade(studentIndex, &average);

        if (error != OK) return error;

        std::cout << "AVG " << words[1] << " "
                  << average << "\n";
        return OK;
    }

    if (words[0] == "GRAD")
    {
        if (count != 2) return E_PARSE;

        int studentIndex = findStudent(words[1]);
        if (studentIndex == -1) return E_UNKNOWN_STUDENT;

        std::string status;
        ErrorCode error =
            getGraduationStatus(studentIndex, &status);

        if (error != OK) return error;

        std::cout << "GRAD " << words[1] << " "
                  << status << "\n";
        return OK;
    }

    if (words[0] == "ELIGIBLE")
    {
        if (count != 4) return E_PARSE;

        int term = 0;

        if (!parseInteger(words[3], &term) ||
            term < 1 || term > 12)
            return E_PARSE;

        int studentIndex = findStudent(words[1]);
        if (studentIndex == -1) return E_UNKNOWN_STUDENT;

        int courseIndex = findCourse(words[2]);
        if (courseIndex == -1) return E_UNKNOWN_COURSE;

        int check =
            checkEligibility(studentIndex, courseIndex, term);

        if (check == 0)
            std::cout << "ELIGIBLE " << words[1] << " "
                      << words[2] << " YES\n";
        else
            std::cout << "ELIGIBLE " << words[1] << " "
                      << words[2] << " NO E" << check << "\n";

        return OK;
    }

    return E_PARSE;
}

ErrorCode ERP::setProgram(const std::string& line)
{
    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count != 6 || programDefined)
        return E_PARSE;

    int total = 0, elective = 0, minimumGPA = 0;
    int maxLoad = 0, thesis = 0;

    if (!parseInteger(words[1], &total) ||
        !parseInteger(words[2], &elective) ||
        !parseGPA(words[3], &minimumGPA) ||
        !parseInteger(words[4], &maxLoad) ||
        !parseInteger(words[5], &thesis))
        return E_PARSE;

    if (total <= 0 || elective > total ||
        maxLoad <= 0 || thesis < 0)
        return E_PARSE;

    program.totalCredits = total;
    program.electiveCredits = elective;
    program.minGPA = minimumGPA;
    program.maxLoad = maxLoad;
    program.thesisCredits = thesis;

    // FIX 2: Mark PROGRAM configuration as defined.
    programDefined = true;

    return OK;
}

ErrorCode ERP::addCourse(const std::string& line)
{
    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count != 6) return E_PARSE;

    if (words[1].empty() || words[1].length() > 10)
        return E_PARSE;

    if (numCourses >= MAX_COURSES)
        return E_FULL;

    if (findCourse(words[1]) != -1)
        return E_DUPLICATE;

    CourseType type;

    if (words[2] == "REGULAR") type = REGULAR;
    else if (words[2] == "SU") type = SU;
    else if (words[2] == "THESIS") type = THESIS;
    else return E_BAD_COURSE;

    int credits = 0;

    if (!parseInteger(words[3], &credits))
        return E_PARSE;

    if (credits > MAX_CREDITS ||
        (type != SU && credits == 0))
        return E_BAD_COURSE;

    CourseCategory category;

    if (words[4] == "CORE") category = CORE;
    else if (words[4] == "ELECTIVE") category = ELECTIVE;
    else return E_BAD_COURSE;

    int prerequisite = -1;

    if (words[5] != "-")
    {
        prerequisite = findCourse(words[5]);

        if (prerequisite == -1 ||
            words[5] == words[1])
            return E_BAD_COURSE;
    }

    courses[numCourses].setData(words[1], type, credits,
                                category, prerequisite);
    numCourses++;

    return OK;
}

ErrorCode ERP::addStudent(const std::string& line)
{
    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count != 3) return E_PARSE;

    if (words[1].empty() || words[1].length() > 10 ||
        words[2].empty() || words[2].length() > 30)
        return E_PARSE;

    if (numStudents >= MAX_STUDENTS)
        return E_FULL;

    if (findStudent(words[1]) != -1)
        return E_DUPLICATE;

    ErrorCode error =
        students[numStudents].initializeRegistrations();

    if (error != OK)
        return error;

    students[numStudents].setData(words[1], words[2]);
    numStudents++;

    return OK;
}