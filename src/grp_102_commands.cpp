// group-102
// group-102

#include "grp_102_erp.h"

// Split a command into words
static int splitWords(const std::string& line,
                      std::string words[],
                      int capacity)
{
    int count = 0;
    int i = 0;

    while (i < static_cast<int>(line.length()))
    {
        while (i < static_cast<int>(line.length()) &&
               (line[i] == ' ' || line[i] == '\t' ||
                line[i] == '\r'))
        {
            i++;
        }

        if (i >= static_cast<int>(line.length()))
        {
            break;
        }

        if (count >= capacity)
        {
            return -1;
        }

        std::string word = "";

        while (i < static_cast<int>(line.length()) &&
               line[i] != ' ' && line[i] != '\t' &&
               line[i] != '\r')
        {
            word += line[i];
            i++;
        }

        words[count] = word;
        count++;
    }

    return count;
}

// Convert a non-negative integer string
static bool parseInteger(const std::string& value, int* result)
{
    if (value.empty() || result == 0)
    {
        return false;
    }

    int number = 0;

    for (int i = 0; i < static_cast<int>(value.length()); i++)
    {
        if (value[i] < '0' || value[i] > '9')
        {
            return false;
        }

        int digit = value[i] - '0';

        if (number > (2147483647 - digit) / 10)
        {
            return false;
        }

        number = number * 10 + digit;
    }

    *result = number;
    return true;
}

// Convert GPA format such as 5.00 into 500
static bool parseGPA(const std::string& value, int* result)
{
    std::size_t dot = value.find('.');

    if (dot == std::string::npos ||
        dot == 0 ||
        dot != value.length() - 3 ||
        result == 0)
    {
        return false;
    }

    std::string whole = value.substr(0, dot);
    std::string fraction = value.substr(dot + 1, 2);

    int first = 0;
    int second = 0;

    if (!parseInteger(whole, &first) ||
        !parseInteger(fraction, &second))
    {
        return false;
    }

    if (first > 10 || second > 99 ||
        (first == 10 && second != 0))
    {
        return false;
    }

    *result = first * 100 + second;
    return true;
}

// Process one input command
ErrorCode ERP::processLine(const std::string& line, int lineNumber)
{
    (void)lineNumber;

    std::string words[7];

    int count = splitWords(line, words, 7);

    if (count == 0)
    {
        return OK;
    }

    if (count < 0)
    {
        return E_PARSE;
    }

    if (words[0][0] == '#')
    {
        return OK;
    }

    if (words[0] == "PROGRAM")
    {
        if (count != 6)
        {
            return E_PARSE;
        }

        return setProgram(line);
    }

    if (words[0] == "COURSE")
    {
        if (count != 6)
        {
            return E_PARSE;
        }

        return addCourse(line);
    }

    if (words[0] == "STUDENT")
    {
        if (count != 3)
        {
            return E_PARSE;
        }

        return addStudent(line);
    }

    return E_PARSE;
}

// PROGRAM command
ErrorCode ERP::setProgram(const std::string& line)
{
    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count != 6)
    {
        return E_PARSE;
    }

    if (programDefined)
    {
        return E_PARSE;
    }

    int total = 0;
    int elective = 0;
    int minimumGPA = 0;
    int maxLoad = 0;
    int thesis = 0;

    if (!parseInteger(words[1], &total) ||
        !parseInteger(words[2], &elective) ||
        !parseGPA(words[3], &minimumGPA) ||
        !parseInteger(words[4], &maxLoad) ||
        !parseInteger(words[5], &thesis))
    {
        return E_PARSE;
    }

    if (total <= 0 || elective < 0 ||
        elective > total || maxLoad <= 0 ||
        thesis < 0)
    {
        return E_PARSE;
    }

    program.totalCredits = total;
    program.electiveCredits = elective;
    program.minGPA = minimumGPA;
    program.maxLoad = maxLoad;
    program.thesisCredits = thesis;

    programDefined = true;

    return OK;
}

// COURSE command
ErrorCode ERP::addCourse(const std::string& line)
{
    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count != 6)
    {
        return E_PARSE;
    }

    if (words[1].length() > 10)
    {
        return E_PARSE;
    }

    if (numCourses >= MAX_COURSES)
    {
        return E_FULL;
    }

    if (findCourse(words[1]) != -1)
    {
        return E_DUPLICATE;
    }

    CourseType type;

    if (words[2] == "REGULAR")
    {
        type = REGULAR;
    }
    else if (words[2] == "SU")
    {
        type = SU;
    }
    else if (words[2] == "THESIS")
    {
        type = THESIS;
    }
    else
    {
        return E_BAD_COURSE;
    }

    int credits = 0;

    if (!parseInteger(words[3], &credits))
    {
        return E_PARSE;
    }

    if (type == SU)
    {
        if (credits < 0 || credits > MAX_CREDITS)
        {
            return E_BAD_COURSE;
        }
    }
    else
    {
        if (credits < 1 || credits > MAX_CREDITS)
        {
            return E_BAD_COURSE;
        }
    }

    CourseCategory category;

    if (words[4] == "CORE")
    {
        category = CORE;
    }
    else if (words[4] == "ELECTIVE")
    {
        category = ELECTIVE;
    }
    else
    {
        return E_BAD_COURSE;
    }

    int prerequisite = -1;

    if (words[5] != "-")
    {
        prerequisite = findCourse(words[5]);

        if (prerequisite == -1)
        {
            return E_BAD_COURSE;
        }
    }

    courses[numCourses].setData(
        words[1],
        type,
        credits,
        category,
        prerequisite
    );

    numCourses++;

    return OK;
}

// STUDENT command
ErrorCode ERP::addStudent(const std::string& line)
{
    std::string words[7];
    int count = splitWords(line, words, 7);

    if (count != 3)
    {
        return E_PARSE;
    }

    if (words[1].length() > 10 ||
        words[2].length() > 30)
    {
        return E_PARSE;
    }

    if (numStudents >= MAX_STUDENTS)
    {
        return E_FULL;
    }

    if (findStudent(words[1]) != -1)
    {
        return E_DUPLICATE;
    }

    ErrorCode error = students[numStudents].initializeRegistrations();

    if (error != OK)
    {
        return error;
    }

    students[numStudents].setData(words[1], words[2]);

    numStudents++;

    return OK;
}
