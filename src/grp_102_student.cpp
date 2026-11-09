// group-102

#include "grp_102_erp.h"

// Constructor
Student::Student()
{
    roll = "";
    name = "";
    regs = 0;
    numRegs = 0;
}

// Destructor
Student::~Student()
{
    delete[] regs;
    regs = 0;
    numRegs = 0;
}

// Allocate registration array
ErrorCode Student::initializeRegistrations()
{
    if (regs != 0)
    {
        return OK;
    }

    regs = new Registration[MAX_REGS];

    if (regs == 0)
    {
        return E_FULL;
    }

    numRegs = 0;
    return OK;
}

// Set student information
void Student::setData(const std::string& studentRoll,
                      const std::string& studentName)
{
    roll = studentRoll;
    name = studentName;
}

// Return roll number
const std::string& Student::getRoll() const
{
    return roll;
}

// Return student name
const std::string& Student::getName() const
{
    return name;
}

// Add a new registration
ErrorCode Student::addRegistration(int courseIndex, int semester)
{
    if (regs == 0)
    {
        return E_FULL;
    }

    if (numRegs >= MAX_REGS)
    {
        return E_FULL;
    }

    regs[numRegs].initialize(courseIndex, semester);
    numRegs++;

    return OK;
}

// Return number of registrations
int Student::getRegistrationCount() const
{
    return numRegs;
}

// Access registration
Registration* Student::getRegistration(int index)
{
    if (index < 0 || index >= numRegs)
    {
        return 0;
    }

    return &regs[index];
}

// Constant registration access
const Registration* Student::getRegistration(int index) const
{
    if (index < 0 || index >= numRegs)
    {
        return 0;
    }

    return &regs[index];
}

// Find registration using course and semester
int Student::findRegistration(int courseIndex, int semester) const
{
    for (int i = 0; i < numRegs; i++)
    {
        if (regs[i].getCourse() == courseIndex &&
            regs[i].getTerm() == semester)
        {
            return i;
        }
    }

    return -1;
}
