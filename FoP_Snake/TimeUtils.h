//Routines that get the system date and time and produce them as a formatted strings
//
//	GetSystemTime: get time from system
//	TimeToString: convert the time to a string in 24-h digital clock format (00:00:00)
//	GetTime: return the current time in a string format (00:00:00)
//	GetSystemDate: get date from system
//	DateToString: convert the date to a string in format (dd/mm/yyyy)
//	GetDate: return the current date in format (dd/mm/yyyy)
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef TimeUtilsH
#define TimeUtilsH

#include <ctime>		//for time routines
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

void getSystemTime(int& hrs, int& mins, int& secs); 
//get time from system

const string timeToString(int h, int m, int s); 
//convert the time to a string in 24-h digital clock format (00:00:00)

const string getTime() ; 
//return the current time in a string format

void getSystemDate(int& day, int& month, int& year); 
//get date from system

const string dateToString(int day, int month, int year);
//convert the date to a string in format (dd/mm/yyyy)

const string getDate(); 
//return the current date in a string format

#endif