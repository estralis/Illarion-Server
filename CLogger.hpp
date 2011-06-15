#ifndef CLOGGER_HPP
#define CLOGGER_HPP

#include <map>
#include <boost/shared_ptr.hpp>
#include <sstream>



class CLogger
{
   //public functions/variables
    public:
    
        template<class strconfrom>
        /**
        * funtion which converts a given argument to string
        *
        *@param convert the value which should be converted to a string
        *@return the convert param as a string.
        *
        **/
        static const std::string toString(const strconfrom& convert) 
        {
            std::stringstream stream;
            stream << convert;
            return stream.str();
        }
        
        /**
        *function which returns the current Instance of the singleton pattern or creates a new one
        *
        *@return the current Logger instance or a new instance if there was none
        *
        **/
        static CLogger * get();
        
        /**
        * static function which writes an Error to a Logfile
        *
        *@param LogType a string which holds the type of the Logfile for writing the error
        *@param Message a string which holds the Message which should be logged
        *@param saveTime a boolean value if set to true the time is added to the message otherwise no time is logged
        *
        * For example: 
        *   <b>CLogger::writeError("Log","This is a testlog");</b>
        * <i> creates a error log file called Log_err if this doesn't exist and appends the line "This is a testlog" with the 
        * current date and time </i>
        **/
        static void writeError(std::string LogType, std::string Message, bool saveTime = true);
        
        /**
        * static function which writes a Message to a Logfile
        *
        *@param LogType a string which holds the type of the Logfile for writing the error
        *@param Message a string which holds the Message which should be logged
        *@param saveTime a boolean value if set to true the time is added to the message otherwise no time is logged
        *
        * For example: 
        *   <b>CLogger::writeMessage("Log","This is a testlog");</b>
        * <i> creates a error log file called Log_msg if this doesn't exist and appends the line "This is a testlog" with the 
        * current date and time </i>
        **/
        static void writeMessage(std::string LogType, std::string Message, bool saveTime = true);
        
        /**
        * activates a log 
        * @param LogType the log which should be activated
        */
        static void activateLog(std::string LogType);
        
        /**
        * deactivates a log 
        * @param LogType the log which should be activated
        */
        static void deactivateLog(std::string LogType);
        
    //protected functions/variables
    protected: 
        /**
        *standard constructor of the Logger class
        **/
        CLogger();
        
        /**
        *standard destructor of the Logger class
        **/
        ~CLogger();
        
        
        
    //private functions/variables 
    private:
    
        /**
        * function which searches in the interna CLogger::logs list for a specific Logfile
        *@param LogType a string which holds the type (name) of that log
        *@param pLog a reference to a pointer of an open ofstream
        *@return true if a file was found otherwise false
        **/
        static bool findLog(std::string LogType, boost::shared_ptr<std::ofstream> &pLog);

        /**
        * function which creates a new Logfile and adds it to the internal CLogger::logs list
        *@param LogType a string which holds the type (name) of that log
        *@param pLog a reference to the pointer of the ofstream objekt after it was sucessfully created
        *@return true if the log was created sucessfully otherwise false
        **/
        static bool createLog(std::string LogType, boost::shared_ptr<std::ofstream> &pLog);
        
        /**
        * checks if a log is activated
        * @param LogType the log which should be checked if activated
        * @return true if the log is activated, false if it was not in list or not activated
        */
        static bool isLogActivated(std::string LogType);
        
        /**
        * function which gets the current time and date for log it
        *@return a String value which holds the current date
        **/
        static std::string getLogDate();
        
        /**
        * a private variable which holds the current and only instance of the object (its a singleton class)
        **/
        static CLogger * theLoggerInstance;
        
        /**
        * a private map which holds the Type (name) of a log as the key and a pointer to a open ofstream 
        **/
        typedef std::map<std::string,/* std::ofstream */ boost::shared_ptr<std::ofstream> > LOGMAP;
        typedef std::map<std::string, bool> LOGACTIVATEDMAP;
        
        /**
        * the map of logs 
        */
        static LOGMAP logs;
        static LOGACTIVATEDMAP logact;
        
};
        
#endif

