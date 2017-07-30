#include "Action.h"

Action::Action(std::function<void()> aDo, std::function<void()> aUndo)
{
   Do = aDo;
   Undo = aUndo;
}

Action::~Action()
{

}

void Action::SetIdentifier(std::string aszId)
{
   Identifier = aszId;
}

std::string Action::GetIdentifier()
{
   return Identifier;
}

void Action::Execute()
{
   Do();
}

void Action::Rollback()
{
   Undo();
}