// group-102

#ifndef GRP_102_ERP_H
#define GRP_102_ERP_H

#include <iostream>
#include <string>

const int MAX_STUDENTS = 100;
const int MAX_COURSES = 50;
const int MAX_REGS = 40;
const int MAX_CREDITS = 24;

enum ErrorCode
{
    OK,
    E_PARSE,
    E_FULL,
    E_DUPLICATE,
    E_UNKNOWN_STUDENT,
    E_UNKNOWN_COURSE,
    E_BAD_COURSE,
    E_BAD_GRADE,
    E_GRADE_TYPE,
    E_NOT_REGISTERED,
    E_ALREADY_GRADED,
    E_THESIS_STATE,
    E_ALREADY_PASSED,
    E_ALREADY_REGISTERED,
    E_PREREQ,
    E_CREDIT_LIMIT,
    E_THESIS_NOT_ALLOWED,
    E_NO_GPA
};

enum CourseType
{
    REGULAR,
    SU,
    THESIS
};

enum CourseCategory
{
    CORE,
    ELECTIVE
};

enum ThesisState
{
    NOT_THESIS,
    REGISTERED,
    IN_PROGRESS,
    FINAL,
    FAILED
};

struct ProgramConfig
{
    int totalCredits;
    int electiveCredits;
    int minGPA;
    int maxLoad;
    int thesisCredits;
};

class Course
{
private:
    std::string code;
    CourseType type;
    int credits;
    CourseCategory category;
    int prereq;

public:
    Course();

    void setData(const std::string& courseCode,
                 CourseType courseType,
                 int courseCredits,
                 CourseCategory courseCategory,
                 int prerequisite);

    const std::string& getCode() const;
    CourseType getType() const;
    int getCredits() const;
    CourseCategory getCategory() const;
    int getPrerequisite() const;
    bool isCore() const;
};

class Registration
{
private:
    int course;
    int term;
    std::string grade;
    ThesisState thesis;

public:
    Registration();

    void initialize(int courseIndex, int semester);

    int getCourse() const;
    int getTerm() const;
    const std::string& getGrade() const;
    ThesisState getThesisState() const;

    void setGrade(const std::string& symbol);
    void setThesisState(ThesisState state);
    bool isGraded() const;
};

class Student
{
private:
    std::string roll;
    std::string name;
    Registration* regs;
    int numRegs;

public:
    Student();
    ~Student();

    ErrorCode initializeRegistrations();

    void setData(const std::string& studentRoll,
                 const std::string& studentName);

    const std::string& getRoll() const;
    const std::string& getName() const;

    ErrorCode addRegistration(int courseIndex, int semester);

    int getRegistrationCount() const;

    Registration* getRegistration(int index);
    const Registration* getRegistration(int index) const;

    int findRegistration(int courseIndex, int semester) const;

    Student(const Student&) = delete;
    Student& operator=(const Student&) = delete;
};

class ERP
{
private:
    Course courses[MAX_COURSES];
    Student students[MAX_STUDENTS];

    int numCourses;
    int numStudents;

    ProgramConfig program;
    bool programDefined;

    int findCourse(const std::string& code) const;
    int findStudent(const std::string& roll) const;

    int checkEligibility(int studentIndex,
                         int courseIndex,
                         int term) const;

    bool isCoursePassed(int studentIndex,
                        int courseIndex) const;

    int earnedCredits(int studentIndex) const;

public:
    ERP();
    ~ERP();

    ErrorCode processLine(const std::string& line,
                          int lineNumber);

    ErrorCode setProgram(const std::string& line);
    ErrorCode addCourse(const std::string& line);
    ErrorCode addStudent(const std::string& line);

    ErrorCode registerCourse(int studentIndex,
                             int courseIndex,
                             int term);

    ErrorCode submitGrade(int studentIndex,
                          int courseIndex,
                          int term,
                          const std::string& symbol);

    ErrorCode convertThesis(int studentIndex,
                            int courseIndex,
                            const std::string& letter);

    ErrorCode gpaHundredths(int studentIndex,
                            int* result) const;

    ErrorCode getAverageGrade(int studentIndex,
                              std::string* result) const;

    ErrorCode getGraduationStatus(int studentIndex,
                                  std::string* result) const;

    int gradePoints(const std::string& symbol) const;
    const char* errorName(ErrorCode error) const;
};

#endif
