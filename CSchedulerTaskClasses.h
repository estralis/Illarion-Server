#ifndef CSCHEDULERTASKCLASSES
#define CSCHEDULEDTASKCLASSES

#include "CScheduler.h"
#include "types.h"
#include "TableStructs.hpp"
#include "CCommonObjectTable.h"
#include "script/CLuaLearnScript.hpp"

extern CCommonObjectTable * CommonItems;
extern boost::shared_ptr<CLuaLearnScript>learnScript;

class CSTalk : public CSchedulerObject {

	public:

		CSTalk(TYPE_OF_CHARACTER_ID id, std::string ntext, unsigned short int ncount, unsigned long int ncycles, unsigned long int cycle_time) {
			playerid = id;
			text = ntext;
			CycleTime = cycle_time;
			nextCycle = ncycles;
			count = ncount;
		}
		virtual ~CSTalk() {}

		bool operator()( CWorld* world ) {
			ccharactervector < CPlayer* >::iterator playerIterator;
			for ( playerIterator = world->Players.begin(); playerIterator < world->Players.end(); ++playerIterator ) {
				if ( ( *playerIterator )->id == playerid ) {
					//spieler noch online
					(*playerIterator)->sendMessage( text);
				}
			}
			count--;
			if (count > 0) {
				nextCycle += CycleTime;
				return true;
			} else return false;
		}

	private:

		TYPE_OF_CHARACTER_ID playerid;
		std::string text;

};

//====================================================================================================
class CSIncreaseHealtPoints : public CSchedulerObject {

	public:

		CSIncreaseHealtPoints(TYPE_OF_CHARACTER_ID id, int nvalue, unsigned short int ncount, unsigned long int first_cycle, unsigned long int cycle_time) {
			cid = id; //Characterid f�r sp�tere �berpr�fung
			value = nvalue;
			CycleTime = cycle_time;
			nextCycle = first_cycle;
			count = ncount;
		}

		virtual ~CSIncreaseHealtPoints() {}

		bool operator()( CWorld* world ) {
			CPlayer * cp=NULL;
			ccharactervector < CPlayer* >::iterator playerIterator;
			for ( playerIterator = world->Players.begin(); playerIterator < world->Players.end(); ++playerIterator ) {
				if ( ( *playerIterator )->id == cid ) {
					cp = (*playerIterator); //Spieler gefunden
					break;
				}
			}
			if ( cp!=NULL) {
				if ( cp->battrib.hitpoints + value >= MAXHPS ) {
					//Wenn Heilung mehr als max heilen w�rde dann max heilen und Task entfernen
					cp->battrib.hitpoints = MAXHPS;
					//std::cout<<"Zeiger nicht mehr g�ltig Task gel�scht \n";
					return false;
				} else if ( !cp->IsAlive() )return false; //Wenn Char Tod ist, Task entfernen
				else {
					//std::cout<<"HP erh�hen um : "<<value<<" Cycle: "<<nextCycle<<" n�chste Ausf�hrung: ";
					cp->increaseAttrib("hitpoints",value); //Hp erh�hen
					nextCycle += CycleTime ; //neuen Ausf�hrungszylkus berechnen
					--count; //Ausf�hrung um eins dezimieren
					//std::cout<<nextCycle<<" Count: "<<count<<"\n";
					if ( count <= 0 ) {
						return false; //Wenn langzeiteffekt zuende nicht wieder einf�gen
					} else {
						return true; //Wenn noch ausf�hrungen offen sind Task wieder einf�gen
					}

				}
			} else {
				return false; //spieler nicht mehr online Task l�schen
			}
			//Client updaten
			//cp->sendAttrib("hitpoints",cp->increaseAttrib("hitpoints", 0 ) );

		}

	private:

		TYPE_OF_CHARACTER_ID cid;
		int value;

};
//================================================================================================================
class CSIncreaseManaPoints : public CSchedulerObject {

	public:

		CSIncreaseManaPoints(TYPE_OF_CHARACTER_ID id, int nvalue, unsigned short int ncount, unsigned long int first_cycle, unsigned long int cycle_time) {
			cid = id; //Characterid f�r sp�tere �berpr�fung
			value = nvalue;
			CycleTime = cycle_time;
			nextCycle = first_cycle;
			count = ncount;
		}
		virtual ~CSIncreaseManaPoints() {}

		bool operator()( CWorld* world ) {
			ccharactervector < CPlayer* >::iterator playerIterator;
			CPlayer * cp=NULL;
			for ( playerIterator = world->Players.begin(); playerIterator < world->Players.end(); ++playerIterator ) {
				if ( ( *playerIterator )->id == cid ) {
					cp = (*playerIterator); //Spieler gefunden
					break;
				}
			}
			if ( cp !=NULL ) {
				if ( cp->battrib.mana + value >= MAXMANA ) {
					//Wenn erh�hung h�her w�re als Maxmana dann maxmana und Task entfernen
					cp->battrib.mana = MAXMANA;
					return false;
				} else if ( !cp->IsAlive() )return false; //Wenn Char Tod ist, Task entfernen
				else {
					cp->increaseAttrib("mana",value); //Mana erh�hen
					nextCycle += CycleTime ; //neuen Ausf�hrungszylkus berechnen
					--count; //Ausf�hrung um eins dezimieren
					if ( count <= 0 )return false; //Wenn langzeiteffekt zuende nicht wieder einf�gen
					else return true; //Wenn noch ausf�hrungen offen sind Task wieder einf�gen


				}
			} else {
				return false; //spieler hat ausgeloggt Task entfernen
			}
			//Client updaten
			cp->sendAttrib("mana",cp->increaseAttrib("mana", 0 ) );
		}

	private:

		TYPE_OF_CHARACTER_ID cid;
		CPlayer * ch;
		int value;

};

//=========================================================================================
class CSGlobalPoison : public CSchedulerObject {

	public:

		CSGlobalPoison(unsigned long int first_cycle) {
			nextCycle = first_cycle;
		}
		virtual ~CSGlobalPoison() {}

		bool operator()( CWorld* world ) {
			bool alivebefore=true;
			ccharactervector < CMonster* >::iterator monsterIterator;
			for ( monsterIterator = world->Monsters.begin(); monsterIterator < world->Monsters.end(); ++monsterIterator ) {
				if ( ( *monsterIterator )->getPoisonValue() > 0 ) //Pr�fen ob aktueller Spieler vergiftet ist
				{
					alivebefore = ( *monsterIterator )->IsAlive();
					short int decreasevalue = 1 + ( (*monsterIterator)->increaseAttrib("willpower",0) / 20 ) *  rnd(0,5);
					( *monsterIterator )->increasePoisonValue(-decreasevalue); //Giftwert absenken
					short int schaden = (*monsterIterator)->getPoisonValue() * (80 + rnd(0,40))/(7*(*monsterIterator)->increaseAttrib("constitution",0));
					( *monsterIterator )->increaseAttrib( "hitpoints", -schaden ); //Giftwert * 2 an schaden machen
					if ( !( *monsterIterator )->IsAlive() )  //Wenn Monster nicht mehr lebt
					{
						( *monsterIterator )->setPoisonValue(0); //Vergiftung auf 0 setzen
						if ( alivebefore != ( *monsterIterator )->IsAlive() ) //Wenn er vorher noch gelebt hat
						{
							//Beim Tod durch gift einen Spin anzeigen (Wolke)
							world->sendSpinToAllVisiblePlayers( ( *monsterIterator ) );
							if ( alivebefore ) {
								//Killtime und grund ins Log schreiben
								time_t acttime = time(NULL);
								std::string killtime = ctime(&acttime);
								killtime[killtime.size()-1] = ':';
								kill_log << killtime << " ";
								kill_log << " Monster " << ( *monsterIterator )->name << "(" << ( *monsterIterator )->id << ") killed by Poison effect \n";
							}
						}
					}
				}
			}
			ccharactervector < CPlayer* >::iterator playerIterator;
			for ( playerIterator = world->Players.begin(); playerIterator < world->Players.end(); ++playerIterator ) {
				if ( ( *playerIterator )->getPoisonValue() > 0 ) //Pr�fen ob aktueller Spieler vergiftet ist
				{
					alivebefore = ( *playerIterator )->IsAlive();
					short int decreasevalue = 1 + ( (*playerIterator)->increaseAttrib("willpower",0) / 20 ) *  rnd(0,5);
					( *playerIterator )->increasePoisonValue(-decreasevalue); //Giftwert absenken
					short int schaden = (*playerIterator)->getPoisonValue() * (80 + rnd(0,40))/(7*(*playerIterator)->increaseAttrib("constitution",0));
					( *playerIterator )->increaseAttrib( "hitpoints", -schaden ); //Giftwert * 2 an schaden machen
					if ( !( *playerIterator )->IsAlive() )  //Wenn Spieler nicht mehr lebt
					{
						( *playerIterator )->setPoisonValue(0); //Vergiftung auf 0 setzen
						if ( alivebefore != ( *playerIterator )->IsAlive() ) //Wenn er vorher noch gelebt hat
						{
							//Beim Tod durch gift einen Spin anzeigen (Wolke)
							world->sendSpinToAllVisiblePlayers( ( *playerIterator ) );
							if ( alivebefore ) {
								//Killtime und grund ins Log schreiben
								time_t acttime = time(NULL);
								std::string killtime = ctime(&acttime);
								killtime[killtime.size()-1] = ':';
								kill_log << killtime << " ";
								kill_log << " Player " << ( *playerIterator )->name << "(" << ( *playerIterator )->id << ") killed by Poison effect \n";
							}
						}
					}
				}
			}
			/* Giftschaden bei NPC's deaktiviert
			ccharactervector < CNPC* >::iterator npcIterator;
			for ( npcIterator = world->Npc.begin(); npcIterator < world->Npc.end(); ++npcIterator )
			{
			      //ToDo- Schleife einf�gen um Vergifteten Spielern Schaden zu machen
			}
			*/
			nextCycle += 2; //Neue Zykluszeit berechnen (aller 2 Zyklen);
			return true; //Task erneut einf�gen
		}

};


class CSGlobalPlayerLearnrate : public CSchedulerObject {

	public:

		CSGlobalPlayerLearnrate(unsigned long int first_cycle) {
			nextCycle = first_cycle;
		}
		virtual ~CSGlobalPlayerLearnrate() {}

		bool operator()( CWorld* world ) {
			ccharactervector < CPlayer* >::iterator playerIterator;
			for ( playerIterator = world->Players.begin(); playerIterator < world->Players.end(); ++playerIterator ) {
				if ( ( *playerIterator )->getMentalCapacity() > 0 ) //Pr�fen ob aktueller Spieler MC > 0 hat
				{
					learnScript->reduceMC( *playerIterator );
				}
			}
			nextCycle += 10; //Neue Zykluszeit berechnen (aller 10 Zyklen);
			return true; //Task erneut einf�gen
		}

};

class CSGlobalMonsterLearnrate : public CSchedulerObject {

	public:

		CSGlobalMonsterLearnrate(unsigned long int first_cycle) {
			nextCycle = first_cycle;
		}
		virtual ~CSGlobalMonsterLearnrate() {}
		bool operator()( CWorld* world ) {
			ccharactervector < CMonster* >::iterator monsterIterator;
			for ( monsterIterator = world->Monsters.begin(); monsterIterator < world->Monsters.end(); ++monsterIterator ) {
				if ( ( *monsterIterator )->getMentalCapacity() > 0 ) //Pr�fen ob aktuelles Monster MC > 0 hat
				{
					learnScript->reduceMC( *monsterIterator );
				}
			}

			ccharactervector < CNPC* >::iterator npcIterator;
			for ( npcIterator = world->Npc.begin(); npcIterator < world->Npc.end(); ++npcIterator ) {
				if ( ( *npcIterator )->getMentalCapacity() > 0 ) {
					learnScript->reduceMC( *npcIterator );
				}
			}
			nextCycle += 10; //Neue Zykluszeit berechnen (aller 30 Zyklen) zur Serverentlastung daf�r die dreifache Senkung;
			return true; //Task erneut einf�gen
		}
};

/*
class CSItemScriptCycle : public CSchedulerObject
{
    public:
    
        CSItemScriptCycle(unsigned long int first_cycle) { nextCycle = first_cycle; }
        
        virtual ~CSItemScriptCycle() {}
        
        bool operator() ( CWorld* world )
        {
                CommonItems->NextCycle();
                nextCycle += 10;
                return true;
        }
};
*/

class CSTempAttribCycle : public CSchedulerObject
{
    public:
 
        CSTempAttribCycle(unsigned long int first_cycle) { nextCycle = first_cycle; }
        
        virtual ~CSTempAttribCycle() {}
        
        bool operator() ( CWorld* world)
        {
            ccharactervector< CMonster* >::iterator monsterIterator;
            for ( monsterIterator = world->Monsters.begin(); monsterIterator < world->Monsters.end(); ++monsterIterator )(*monsterIterator)->tempAttribCheck();
            
            ccharactervector< CNPC* >::iterator npcIterator;
            for ( npcIterator = world->Npc.begin(); npcIterator < world->Npc.end(); ++npcIterator)(*npcIterator)->tempAttribCheck();
            
           ccharactervector < CPlayer* >::iterator playerIterator;
            for ( playerIterator = world->Players.begin(); playerIterator < world->Players.end(); ++playerIterator ) (*playerIterator )->tempAttribCheck();
            
            nextCycle += 1;
            return true;
           
        }
};
#endif
