#include <boost/enable_shared_from_this.hpp>
#include "netinterface/CBasicClientCommand.hpp"
#include "netinterface/protocol/ClientCommands.hpp"
#include "CCommandFactory.hpp"


CNetInterface::CNetInterface(boost::asio::io_service &io_servicen) : online( false ), io_service(io_servicen), socket(io_servicen), inactive(0)
{
    cmd.reset();
}

std::string CNetInterface::getIPAdress()
{
    return ipadress;
}


CNetInterface::~CNetInterface()
{
    try
    {
        //std::cout<<"destructing new netinterface"<<std::endl;
        online = false;
        sendQueue.clear();
        receiveQueue.clear();
		socket.close();   
        //std::cout<<"destruction done"<<std::endl;
    }
    catch (std::exception e)
    {
        std::cerr << e.what () << std::endl;
    }
    catch (...)
    {} 

}



void CNetInterface::closeConnection()
{
    
    //std::cerr<<getIPAdress()<<"closeConnection called"<<std::endl;
    online = false;
}

bool CNetInterface::activate()
{
    try
    {
        boost::asio::async_read(socket,boost::asio::buffer(headerBuffer,6), boost::bind(&CNetInterface::handle_read_header, shared_from_this(),boost::asio::placeholders::error));
        ipadress = socket.remote_endpoint().address().to_string();
        online = true;
        return true;
    } 
    catch( std::exception ex )
    {
        std::cerr<<"error during activate: "<<ex.what()<<std::endl;
        return false;
    }
}


void CNetInterface::handle_read_data(const boost::system::error_code& error)
{
    if ( !error )
    {
        if ( online )
        {
            cmd->decodeData();
            //std::cout<<"data decoded"<<std::endl;
            if ( cmd->isDataOk() )
            {
                //std::cout<<"data was ok"<<std::endl;
                receiveQueueMutex.lock();
                receiveQueue.push_back( cmd );
                receiveQueueMutex.unlock();
            }
            else 
            {
                std::cout<<"error receiving command"<<std::endl;
            }
            //std::cout<<"cmd resetted!"<<std::endl;
            cmd.reset();
            boost::asio::async_read(socket,boost::asio::buffer(headerBuffer,6), boost::bind(&CNetInterface::handle_read_header, shared_from_this(),boost::asio::placeholders::error));
        }
    }
    else
    {
        if (online)std::cerr<<"handle_read_data error during read "<<getIPAdress()<<" "<<error.message()<<" :"<< error<<std::endl;
        closeConnection();
        boost::asio::async_read(socket,boost::asio::buffer(headerBuffer,6), boost::bind(&CNetInterface::handle_read_header, shared_from_this(),boost::asio::placeholders::error));
    }
}

bool CNetInterface::nextInactive()
{
    inactive++;
    return ( inactive > 1000 );
}

void CNetInterface::handle_read_header(const boost::system::error_code& error)
{
    if ( !error )
    {
        if ( (headerBuffer[0] xor 255) == headerBuffer[1])
        {
            //Correcter header decodieren und comand empfangen
            uint16_t length = headerBuffer[2] << 8;
            length = length | headerBuffer[3];
            uint16_t checkSum = headerBuffer[4] << 8;
            checkSum = checkSum | headerBuffer[5];
            cmd = commandFactory.getCommand(headerBuffer[0]);
            if ( cmd )
            {
                cmd->setHeaderData(length,checkSum);
                boost::asio::async_read(socket,boost::asio::buffer(cmd->msg_data(),cmd->getLength()), boost::bind(&CNetInterface::handle_read_data, shared_from_this(),boost::asio::placeholders::error));
                return;
            }
            else
            {
                std::cout<<"No Command with id "<<headerBuffer[0]<<"found searching new Command"<<std::endl;
            }
        }
        //Kein Correcter Header
        std::cout<<"wrong header searching for command id"<<std::endl;
        //Suchen im Header nach einer Command ID
        for ( int i = 1 ; i < 5; ++i)
        {
            //Korrekte Command ID gefunden
            if ( (headerBuffer[i] xor 255) == headerBuffer[i+1] )
            {
                std::cout<<"found correct command id at pos "<<i<<std::endl;
                //copy the rest of the correct message to the start of the buffer
                int start = 0;
                while ( i < 6 )
                {
                    headerBuffer[start++] = headerBuffer[i++];
                }
                //restheader empfangen
                boost::asio::async_read(socket,boost::asio::buffer(&headerBuffer[start],6-start), boost::bind(&CNetInterface::handle_read_header, shared_from_this(),boost::asio::placeholders::error));
                return;
            }
        }
        //Keine Command Signature gefunden wieder 6 Byte Header auslesen
        boost::asio::async_read(socket,boost::asio::buffer(headerBuffer,6), boost::bind(&CNetInterface::handle_read_header, shared_from_this(),boost::asio::placeholders::error));

    }
    else
    {
        if (online)std::cerr<<"handle_read_header error during read "<<getIPAdress()<<" "<<error.message()<<" :"<< error<<std::endl;
        closeConnection();
        
    }
}


void CNetInterface::addCommand( boost::shared_ptr<CBasicServerCommand> command )
{
    if ( online )
    {
        command->addHeader();
        sendQueueMutex.lock();
        bool write_in_progress = !sendQueue.empty();
        sendQueue.push_back(command);
        try
        {
            if (!write_in_progress && online)
            {
                boost::asio::async_write(socket,boost::asio::buffer(sendQueue.front()->cmdData(),sendQueue.front()->getLength()),
                    boost::bind(&CNetInterface::handle_write, shared_from_this(),boost::asio::placeholders::error));
            }
        }
        catch ( std::exception ex )
        {
            std::cerr<<"addCommand error during write: "<<ex.what()<<std::endl;
            closeConnection();
        }
        sendQueueMutex.unlock();
    }

}

void CNetInterface::shutdownSend( boost::shared_ptr<CBasicServerCommand> command )
{
	try
	{
		command->addHeader();
		shutdownCmd = command;
		boost::asio::async_write(socket,boost::asio::buffer(shutdownCmd->cmdData(),shutdownCmd->getLength()),
		boost::bind(&CNetInterface::handle_write_shutdown, shared_from_this(),boost::asio::placeholders::error));
	}
	catch ( std::exception ex )
	{
		std::cerr<<"Exception beim Schreiben von Daten:"<<ex.what()<<std::endl;
		closeConnection();
	}
}

void CNetInterface::handle_write(const boost::system::error_code& error)
{
	try
	{
		if ( !error )
		{
            if ( online )
            {
                sendQueueMutex.lock();
                sendQueue.pop_front();
                if ( !sendQueue.empty() && online)
                {
                    boost::asio::async_write(socket,boost::asio::buffer(sendQueue.front()->cmdData(),sendQueue.front()->getLength()),
                        boost::bind(&CNetInterface::handle_write, shared_from_this(),boost::asio::placeholders::error));
                }
                sendQueueMutex.unlock();
                
            }
		}
		else
		{    
            std::cerr<<"handle_write error during write: "<<error.message()<<" :"<<error<<std::endl;
			closeConnection();
		}
	}
	catch ( std::exception ex )
	{
        std::cout<<"caugth Exception on handle_write"<<ex.what()<<std::endl;
		closeConnection();
	}
}

void CNetInterface::handle_write_shutdown(const boost::system::error_code& error)
{
    if ( !error )
    {
        closeConnection();
        shutdownCmd.reset();
    }
    else
    {
        if (online)std::cerr<<"handle_write_shutdown error during write: "<<error.message()<<" :"<<error<<std::endl;
        closeConnection();
    }
}

boost::shared_ptr<CBasicClientCommand> CNetInterface::getCommand()
{
    if ( online )
    {
        boost::shared_ptr<CBasicClientCommand> ret;
        receiveQueueMutex.lock();
        if ( !receiveQueue.empty() )
        {
            ret = receiveQueue.front();
            receiveQueue.pop_front();
        }
        receiveQueueMutex.unlock();
        return ret;
    }
    return boost::shared_ptr<CBasicClientCommand>();
}

