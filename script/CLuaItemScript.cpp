#include "CLuaItemScript.hpp"

#include "luabind/luabind.hpp"
#include <iostream>

#include "CField.hpp"
#include "CPlayer.hpp"
#include "CWorld.hpp"
#include "CLogger.hpp"
#include "fuse_ptr.hpp"

CLuaItemScript::CLuaItemScript(std::string filename, CommonStruct comstr) throw(ScriptException)
		: CLuaScript(filename) , _comstr(comstr) {
	init_functions();
}

CLuaItemScript::~CLuaItemScript() throw() {}

void CLuaItemScript::init_functions() {
	luabind::object globals = luabind::globals(_luaState);
	globals["thisItem"] = _comstr;
}

void CLuaItemScript::UseItem(CCharacter * User, ScriptItem SourceItem, ScriptItem TargetItem, unsigned short counter, unsigned short int param, unsigned char ltastate ) 
{
	try 
    {
        CWorld::get()->setCurrentScript( this );        
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("UseItem called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","UseItem called for: " + CLogger::toString(_comstr.id));
std::cout << "USEITEMCALL: " << (getFileName() + ".UseItem").c_str() << std::endl;
		fuse_ptr<CCharacter> fuse_User(User);
        call("UseItem")( fuse_User, SourceItem, TargetItem, counter, param, ltastate );
        
	} catch (luabind::error &e) 
    {
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("Error: UseItem called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
	writeErrorMsg();
	}
}

bool CLuaItemScript::NextCycle()
{
    try 
    {
        CWorld::get()->setCurrentScript( this ); 
        /*
        CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("NextCycle called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","Nextcycle called for: " + CLogger::toString(_comstr.id));*/
        call("NextCycle")();
        return true;
    }
    catch ( luabind::error &e)
    {
        /*
        CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("Error: NextCycle called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg();*/
        return false;
    }
    return false;
}

void CLuaItemScript::UseItemWithCharacter(CCharacter * User, ScriptItem SourceItem, CCharacter * character, unsigned short counter, unsigned short int param, unsigned char ltastate ) {
	try 
    {
        CWorld::get()->setCurrentScript( this ); 
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("UseItemWithCharacter called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","UseItemWithCharacter called for: " + CLogger::toString(_comstr.id));
		fuse_ptr<CCharacter> fuse_User(User);
        fuse_ptr<CCharacter> fuse_character(character);
        call("UseItemWithCharacter")( fuse_User, SourceItem, fuse_character, counter, param, ltastate );
	} catch (luabind::error &e) {
        //_world->monitoringClientList->sendCommand( new CSendMessageTS("Error: UseItemWithCharacter called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg();
	}
}

void CLuaItemScript::UseItemWithField(CCharacter * User, ScriptItem SourceItem, position TargetPos, unsigned short counter, unsigned short param, unsigned char ltastate) {
	try 
    {
        CWorld::get()->setCurrentScript( this ); 
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("UseItemWithField called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","UseItemWithField called for: " + CLogger::toString(_comstr.id));
		fuse_ptr<CCharacter> fuse_User(User);
        call("UseItemWithField")( fuse_User, SourceItem, TargetPos, counter, param, ltastate );
	} 
    catch (luabind::error &e) 
    {
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("Error: UseItemWithField called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg();
	}
}

bool CLuaItemScript::actionDisturbed(CCharacter * performer, CCharacter * disturber)
{
    try
    {
        CWorld::get()->setCurrentScript( this ); 
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("actionDisturbed called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","actionDisturbed called for: " + CLogger::toString(_comstr.id));
        fuse_ptr<CCharacter> fuse_performer(performer);
        fuse_ptr<CCharacter> fuse_disturber(disturber);
        return luabind::object_cast<bool>(call("actionDisturbed")( fuse_performer, fuse_disturber ));
    }
    catch ( luabind::error &e)
    {
        return true;
    }
}

bool CLuaItemScript::LookAtItem(CCharacter * who, ScriptItem t_item) {
	try 
    {
        CWorld::get()->setCurrentScript( this ); 
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("LookAtItem called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","LookAtItem called for: " + CLogger::toString(_comstr.id));
    	fuse_ptr<CCharacter> fuse_who(who);
        call("LookAtItem")( fuse_who, t_item );
		return true;
	} 
    catch (luabind::error &e) 
    {
		//CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("Error: LookAtItem called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg();
        return false;
	}
	return false;
}

bool CLuaItemScript::MoveItemBeforeMove(CCharacter * who, ScriptItem sourceItem,ScriptItem targetItem) {
	try 
    {
        CWorld::get()->setCurrentScript( this ); 
         //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("MoveItemBeforeMove called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","MoveItemBeforeMove called for: " + CLogger::toString(_comstr.id));        
        fuse_ptr<CCharacter> fuse_who(who);
		return luabind::object_cast<bool>(call("MoveItemBeforeMove")( fuse_who, sourceItem, targetItem ));
	} catch (luabind::error &e) {
		//_world->monitoringClientList->sendCommand( new CSendMessageTS("Error: MoveItemBeforeMove called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg();
	}
	return true;
}

void CLuaItemScript::MoveItemAfterMove(CCharacter * who, ScriptItem sourceItem, ScriptItem targetItem) {
	try 
    {
        CWorld::get()->setCurrentScript( this ); 
         //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("MoveItemAfterMove called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","MoveItemAfterMove called for: " + CLogger::toString(_comstr.id));
		fuse_ptr<CCharacter> fuse_who(who);
        call("MoveItemAfterMove")( fuse_who, sourceItem, targetItem );
	} 
    catch (luabind::error &e) 
    {
		//CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("Error: MoveItemAfterMove called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg();        
	}
}

void CLuaItemScript::CharacterOnField(CCharacter * who) {
	try 
    {
        CWorld::get()->setCurrentScript( this ); 
        //CWorld::get()->monitoringClientList->sendCommand( new CSendMessageTS("CharacterOnField called for: " + CLogger::toString(_comstr.id ),3));
        CLogger::writeMessage("scripts","CharacterOnField called for: " + CLogger::toString(_comstr.id));
		fuse_ptr<CCharacter> fuse_who(who);
        call("CharacterOnField")( fuse_who );
	} catch (luabind::error &e) {
		//_world->monitoringClientList->sendCommand( new CSendMessageTS("Error: CharacterOnField called for: " + CLogger::toString(_comstr.id ) + " " + e.what(),3));
        writeErrorMsg(); 
	}
}


