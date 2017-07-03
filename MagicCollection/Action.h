#pragma once

#include <string>
#include <functional>

/* Class Action (Used In Collection.cpp)
* Used to wrap an action that changes the collection with an action
* that can undo it. Allows undo button.
* Also maintains a string description of the action so that it can
* be recorded in the .history.txt file.
* This class acts as a data structure.
*/
class Action
{
public:
	Action(std::function<void()> aDo, std::function<void()> aUndo);
	~Action();

	void SetIdentifier(std::string aszId);

	void Execute();
	void Rollback();

private:
	std::string Identifier;
	std::function<void()> Do;
	std::function<void()> Undo;
};