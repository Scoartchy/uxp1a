#include <cstring>
#include <list>
#include <variant>
#include <ctime>
#include <chrono>
#include <exception>

#include "sync.h"

std::string LINDA_FILE;
std::string TEMP_FILE;


enum TypeOfElement
{
	intType, floatType, stringType
};



/* Classes */



class Tuple
{
public:
	std::list<std::variant<int, float, std::string>> tupleElements;
};

struct Element
{
	std::string type;
	std::string oper;
	std::string cond;

	Element(std::string t, std::string o, std::string c) : type(t), oper(o), cond(c) {}
};

class TuplePattern
{
public:
	std::list<Element> tuplePatternElements;
};



/* Functions */



/* Converting Tuple to string */
std::string tupleToString(Tuple tuple)
{
	std::string tupleString;

	/* Adding tuple to list of tuples */
	for(std::variant<int, float, std::string> element : tuple.tupleElements)
	{
		/* Check the type of variant variable. */
		if (std::holds_alternative<int>(element))
		{
		 	tupleString += (std::to_string(std::get<int>(element)) + " ");
		}
		else if (std::holds_alternative<float>(element))
		{
			tupleString += (std::to_string(std::get<float>(element)) + " ");
		}
		else if (std::holds_alternative<std::string>(element))
		{
			tupleString += ("\"" + std::get<std::string>(element) + "\" ");
		}
	}

	tupleString.pop_back();

	std::cout << "Tuple: " << tupleString << std::endl; 

	return tupleString;
}


/* Converting string to Tuple */
Tuple stringToTuple(std::string line)
{
	int i = 0;
	bool isInString = false;

	Tuple tuple;
	std::string tupleElement;

	while(i < line.size())
	{
		while(isInString || line[i] != ' ')
		{
			if(line[i] == '"' && !isInString)
			{
				++i;
				isInString = true;
			}
			else if(line[i] == '"' && isInString)
			{
				isInString = false;
			}

			if(line[i] != '"')
				tupleElement.push_back(line[i]);
				
			if(i < line.size())
				++i;
			else 
				break;
		}

		std::cout << "Tuple element: " << tupleElement;

		try 
		{	
			if(tupleElement.find(".") != std::string::npos)
			{
				throw std::exception();
			}
			int i = stoi(tupleElement);
			tuple.tupleElements.push_back(i);
		}
		catch (std::exception& e)
		{
			try 
			{
				float f = stof(tupleElement);
				tuple.tupleElements.push_back(f);
			}
			catch (std::exception e)
			{
				tuple.tupleElements.push_back(tupleElement);
			}
		}
		std::cout << std::endl;
		tupleElement.clear();	

		if(i < line.size())	
			++i;
		else
			break;
	}

	return tuple;
}


extern "C" void output(const char* tupleString)
{
	get_file_access();

	Tuple tuple = stringToTuple(std::string(tupleString));

	std::fstream file;
	file.open(LINDA_FILE.c_str(), std::ios::in | std::ios::out | std::ios::ate); 
	openFileInfo(file, LINDA_FILE);	

	/* Adding tuple to list of tuples */
	for(std::variant<int, float, std::string> element : tuple.tupleElements)
	{
		/* Check the type of variant variable. */
		if (std::holds_alternative<int>(element))
		{
			std::cout << std::get<int>(element) << " ";
		 	file << std::get<int>(element) << " ";
		}
		else if (std::holds_alternative<float>(element))
		{
			std::cout << std::get<float>(element) << " ";
			file << std::get<float>(element) << " ";
		}
		else if (std::holds_alternative<std::string>(element))
		{
			std::cout << std::get<std::string>(element) << " ";
	 		file << "\"" << std::get<std::string>(element) << "\" ";
		}	
	}

	std::cout << "Tuple was " << std::string(tupleString) << " added to LINDA_FILE." << std::endl;

	/* Go to new line */
	file << '\n';
	
	file.close();	

	give_file_access();
}


/* Converting string to TuplePattern */
TuplePattern strintToTuplePattern(std::string pattern)
{
	TuplePattern tuplePattern;

	int i = 0;
	
	while(i < pattern.size())
	{
		std::string type;
		while(pattern[i] != ':' && i < pattern.size())
		{
			type.push_back(pattern[i]);
			++i;
		}
			
		if(type != "integer" && type != "string" && type != "float")
		{
			std::cout << "Error in a tuple pattern" << std::endl;
		}

		std::string oper;
		while((pattern[i] == ':' || pattern[i] == '=' || pattern[i] == '<' || pattern[i] == '>') && i < pattern.size())
		{
			oper.push_back(pattern[i]);
			++i;
		}

		std::string cond;
		if (pattern[i] == '"') 
		{
			++i;
		}
		while(pattern[i] != ',' && i < pattern.size())
		{
			cond.push_back(pattern[i]);
			++i;
		}
		if(cond[cond.size() - 1] == '"') 
		{
			cond = cond.substr(0, cond.size() - 1);
		}

		/* Now comma and space */
		i+=2;

		Element element(type, oper, cond);
		tuplePattern.tuplePatternElements.push_back(element);

		std::cout << "Type: " << type << " Operator: " << oper << " Condition: " << cond << std::endl;
	}

	return tuplePattern;
}


bool compareTupleWithTuplePattern(Tuple& tuple, TuplePattern tuplePattern)
{
	/* Tuple and tuplePattern must have the same size.*/
	if(tuple.tupleElements.size() != tuplePattern.tuplePatternElements.size())
	{
		return false;
	}

	std::list<Element>::const_iterator iterator = tuplePattern.tuplePatternElements.begin();
	int firstNumber, secondNumber;

	/* Comparing all elements of tuple with pattern */
	for(std::variant<int, float, std::string> element : tuple.tupleElements)
	{	
		TypeOfElement typeOfElement;

		try 
		{
			int i = std::get<int>(element);
			typeOfElement = intType;
			std::cout << "Int: " << std::get<int>(element) << std::endl;
		}
		catch (const std::bad_variant_access&) 
		{
			try 
			{
				float f = std::get<float>(element);
				typeOfElement = floatType;
				std::cout << "Float: " << std::get<float>(element) << std::endl;
			}
			catch (const std::bad_variant_access&)
			{
				std::string s = std::get<std::string>(element);
				typeOfElement = stringType;
				std::cout << "String: " << std::get<std::string>(element) << std::endl;
			}
		}

		std::cout << "Type: " << (*iterator).type << std::endl;
		if(typeOfElement == intType && (*iterator).type == "integer")
		{
			std::cout << "Oper: " << (*iterator).oper << std::endl;
			if((*iterator).oper == ":")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
				}	continue;
				
				firstNumber = std::get<int>(element);
				secondNumber = std::stoi((*iterator).cond);

				if(firstNumber == secondNumber)
				{
					++iterator;
					continue;
				}
			}
			else if((*iterator).oper == ":<")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}

				firstNumber = std::get<int>(element);
				secondNumber = std::stoi((*iterator).cond);

				if(firstNumber < secondNumber)
				{
					++iterator;
					continue;
				}
			} 
			else if((*iterator).oper == ":<=")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}
				
				firstNumber = std::get<int>(element);
				secondNumber = std::stoi((*iterator).cond);

				if(firstNumber <= secondNumber)
				{
					++iterator;
					continue;
				}
			}
			else if((*iterator).oper == ":>")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}
				
				firstNumber = std::get<int>(element);
				secondNumber = std::stoi((*iterator).cond);

				if(firstNumber > secondNumber)
				{
					++iterator;
					continue;
				}
			}
			else if((*iterator).oper == ":>=")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}
				
				firstNumber = std::get<int>(element);
				secondNumber = std::stoi((*iterator).cond);

				if(firstNumber >= secondNumber)
				{
					++iterator;
					continue;
				}
			}
			
			return false;
		}
		else if(typeOfElement == floatType && (*iterator).type == "float")
		{
			std::cout << "Oper: " << (*iterator).oper << std::endl;
			
			if((*iterator).oper == ":<")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}

				firstNumber = std::get<float>(element);
				secondNumber = std::stof((*iterator).cond);

				if(firstNumber < secondNumber)
				{
					++iterator;
					continue;
				}
			} 
			else if((*iterator).oper == ":<=")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}
				
				firstNumber = std::get<float>(element);
				secondNumber = std::stof((*iterator).cond);

				if(firstNumber <= secondNumber)
				{
					++iterator;
					continue;
				}
			}
			else if((*iterator).oper == ":>")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}
				
				firstNumber = std::get<float>(element);
				secondNumber = std::stof((*iterator).cond);

				if(firstNumber > secondNumber)
				{
					++iterator;
					continue;
				}
			}
			else if((*iterator).oper == ":>=")
			{
				std::cout << "Cond: " << (*iterator).cond << std::endl;
				if((*iterator).cond == "*")
				{
					++iterator;
					continue;
				}
				
				firstNumber = std::get<float>(element);
				secondNumber = std::stof((*iterator).cond);

				if(firstNumber >= secondNumber)
				{
					++iterator;
					continue;
				}
			}
			
			return false;
		}
		else if(typeOfElement == stringType && (*iterator).type == "string")
		{
			std::string operat = (*iterator).oper;
			const char * tupleString = std::get<std::string>(element).c_str();
			const char * patternString = (*iterator).cond.c_str();

			if (strcmp(patternString,"*") == 0) 
			{
			}
			else if (operat == ":") 
			{
				if (!(strcmp(tupleString, patternString) == 0)) 
				{
					return false;
				}
			}
			else if (operat == ":<") 
			{
				if (!(strcmp(tupleString, patternString) < 0)) 
				{
					return false;
				}
			}
			else if (operat == ":>") 
			{
				if (!(strcmp(tupleString, patternString) > 0)) 
				{
					return false;
				}
			}
			else if (operat == ":<=") 
			{
				if (!(strcmp(tupleString, patternString) <= 0)) 
				{
					return false;
				}
			}
			else if (operat == ":>=") {
				if (!(strcmp(tupleString, patternString) >= 0)) 
				{
					return false;
				}
			}
			++iterator;
		}
		
	}

	return true;
}


bool findTuple(Tuple& t, TuplePattern tuplePattern, unsigned long& lineNum, bool deleteAfterFind)
{
	std::fstream file;
	file.open(LINDA_FILE.c_str(), std::ios::in); 
	openFileInfo(file, LINDA_FILE);

	std::fstream tempFile; 		/* I have to declare it here, so it will be visible out of the scope. */
	bool tupleForInputAlreadyFound = false; 	/* I need to know if tuple was found, so I return true after rewriting whole file. */
	if (deleteAfterFind)
	{
		/* Create temporary file in case we need to remove line, so we can swap it later with the original one. */
		tempFile.open(TEMP_FILE.c_str(), std::ios::out);
		openFileInfo(tempFile, TEMP_FILE);
	}

	bool tupleFinded = false;

	std::string line;
	while(getline(file, line).good()) /* Read file line by line */
	{
		if (!tupleForInputAlreadyFound) /* In case it's runned by "input" function, we must stop incrementing line's number after foudning tuple. */
		{
			++lineNum;
		}
		std::cout << "Line: " << line << std::endl;
		 
		Tuple tuple = stringToTuple(line);
		t = tuple;
		if (!tupleForInputAlreadyFound) /* We already found pattern - no need for further check. */
		{
			tupleFinded = compareTupleWithTuplePattern(tuple, tuplePattern);
		}

		if(tupleFinded && !tupleForInputAlreadyFound) /* Second condition is neccessary, so we stop getting tuples, after finding first matching one. */
		{
			std::cout << "Tuple "<< line << " was found!" << std::endl;
			if (!deleteAfterFind)
			{
				file.close();
				return true;
			}
			else
			{
				tupleForInputAlreadyFound = true; /* We need still to read rest of the lines from file to temp file, so we cannot just return here. */
			}
		}
		else
		{
			std::cout << "Tuple was not found!" << std::endl;
			if (deleteAfterFind) /* We must update tempFile for input operation. */
			{
				tempFile << line << '\n';
			}
		}
	}


	file.close();
	if (deleteAfterFind) 
	{	
		std::cout << "Tuple was " << line << " remove form " << LINDA_FILE << std::endl;
		tempFile.close();
		if (tupleForInputAlreadyFound) 
		{
			/* TEMP_FILE is becoming LINDA_FILE */
			remove(LINDA_FILE.c_str()); 
			rename(TEMP_FILE.c_str(), LINDA_FILE.c_str());
			return true;
		}
		else
		{
			remove(TEMP_FILE.c_str());
		}
	}
	return false;
}

Tuple getDataFromFile(TuplePattern tuplePattern, int timeout, bool isInputOperation)
{
	unsigned long lineNum = 0;
	Tuple tuple;

	/* Trying find a tuple */
	if(findTuple(tuple, tuplePattern, lineNum, isInputOperation))
	{
		std::cout << std::endl;
		for(std::variant<int, float, std::string> element : tuple.tupleElements)
		{
			/* Check the type of variant variable. */
			if (std::holds_alternative<int>(element))
			{
				std::cout << std::get<int>(element) << " ";
			}
			else if (std::holds_alternative<float>(element))
			{
				std::cout << std::get<float>(element) << " ";
			}
			else if (std::holds_alternative<std::string>(element))
			{
				std::cout << std::get<std::string>(element) << " ";
			}
		}

		std::cout << std::endl << "Number of line: " << lineNum << std::endl;
	}

	return tuple;
}



Tuple waitingForAction(TuplePattern tuplePattern, int timeout, bool typeOfAction)
{
	Tuple tuple;
	
	/* Timers */
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	
	/* Keeping time */
	while((end - start).count() < timeout && tuple.tupleElements.size() == 0)
	{
		get_file_access();
		tuple = getDataFromFile(tuplePattern, timeout, typeOfAction);
		give_file_access();
		end = std::chrono::system_clock::now();
	}

	/* Empty list, so tuple was not found. */
	if(tuple.tupleElements.size() == 0)
		std::cout << "Tuple was not found!" << std::endl;

	return tuple;
}

extern "C" const char* input(const char* tuplePatternString, int timeout)
{
	TuplePattern tuplePattern = strintToTuplePattern(std::string(tuplePatternString));
	Tuple tuple = waitingForAction(tuplePattern, timeout, true);
	return tupleToString(tuple).c_str();
}


extern "C" const char* read(const char* tuplePatternString, int timeout)
{
	TuplePattern tuplePattern = strintToTuplePattern(std::string(tuplePatternString));
	Tuple tuple = waitingForAction(tuplePattern, timeout, false);
	return tupleToString(tuple).c_str();
}


int main()
{
	std::cout<<"UXP1A"<<std::endl;

	readConfig();

	std::string test = "13 0.7 \"EITI\" ";
	
	Tuple tuple = stringToTuple(test);
	//output(tuple);
	//tupleToString(tuple);

	std::cout << std::endl;

	//Test parse function
	//TuplePattern tuplePattern = strintToTuplePattern("integer:*, float:>5.5, string:\"abc\""); /*std::string:*,*/

	//TuplePattern tuplePattern = strintToTuplePattern("integer:*, integer:>5, integer:<3");

	//TuplePattern tuplePattern = strintToTuplePattern("integer:*, integer:>5, float:<3, float:<=100.5");
	//TuplePattern tuplePattern = strintToTuplePattern("string:<=\"EZTI\"");

	//std::cout << std::endl;
	//tuple = input(tuplePattern, 3);

	return 0;
}

extern "C" void init_linda(){
	_init_linda();
}
extern "C" void exit_linda(){
	_exit_linda();
}
