#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <variant>
#include <typeinfo>
#include <cstring>


const std::string LINDA_FILE = "test.txt";  /* change to -> "/working_dir/linda_file";*/
const std::string TEMP_FILE = "temp.txt";   /* similarily as above... */


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



void openFileInfo(std::fstream &file)
{	
	if(file.bad())
	{
		std::cout << "ERROR: The file " << LINDA_FILE.c_str() << " could not be found. Program will be terminated." << std::endl;
		std::cout << "Press any key to continue..." << std::endl;
		std::cin.get();
		exit(1);
	}
	else
	{
		std::cout << "The file " << LINDA_FILE.c_str() << " found and opened." << std::endl;
	}
}

void output(Tuple tuple)
{
	/* Test tuple */
	tuple.tupleElements.push_back(13);
	tuple.tupleElements.push_back(0.7f);
	tuple.tupleElements.push_back("EITI");

	std::fstream file;
	file.open(LINDA_FILE.c_str(), std::ios::in | std::ios::out | std::ios::ate); 
	openFileInfo(file);	

	short int typeOfElement = 0;

	/* Adding tuple to list of tuples */
	for(std::variant<int, float, std::string> element : tuple.tupleElements)
	{
		/* Try get int */
		try 
		{
			std::cout << std::get<int>(element) << " ";
			++typeOfElement;
			file << std::get<int>(element) << " ";

		}
		catch (const std::bad_variant_access&) 
		{
			/* Try get float */
			try 
			{
				std::cout << std::get<float>(element) << " ";
				++typeOfElement;
				file << std::get<float>(element) << " ";
			}
			catch (const std::bad_variant_access&) /* So it must be string */
			{
				std::cout << std::get<std::string>(element) << " ";
				++typeOfElement;
				file << "\"" << std::get<std::string>(element) << "\" ";
			}
		}		
	}

	std::cout << "Tuple was added to LINDA_FILE." << std::endl;

	/* Go to new line */
	file << '\n';
	
	file.close();	
}


TuplePattern parsePattern(std::string pattern)
{
	TuplePattern tuplePattern;

	int i = 0;
	
	while(i < pattern.length())
	{
		std::string type;
		while(pattern[i] != ':' && i < pattern.length())
		{
			type.push_back(pattern[i]);
			++i;
		}
			
		if(type != "integer" && type != "string" && type != "float")
		{
			std::cout << "Error in a tuple pattrern" << std::endl;
		}

		std::string oper;
		while((pattern[i] == ':' || pattern[i] == '=' || pattern[i] == '<' || pattern[i] == '>') && i < pattern.length())
		{
			oper.push_back(pattern[i]);
			++i;
		}

		std::string cond;
		if (pattern[i] == '"') 
		{
			++i;
		}
		while(pattern[i] != ',' && i < pattern.length())
		{
			cond.push_back(pattern[i]);
			++i;
		}
		if(cond[cond.length() - 1] == '"') 
		{
			cond = cond.substr(0, cond.length() - 1);
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
	if(tuple.tupleElements.size() != tuplePattern.tuplePatternElements.size())
	{
		return false;
	}

	std::list<Element>::const_iterator iterator = tuplePattern.tuplePatternElements.begin();
	int firstNumber, secondNumber;

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
			std::cout << "Moj test: \n";
			std::cout << "+++" << tupleString << "+++\n";
			std::cout << "+++" << patternString << "+++\n";
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
	openFileInfo(file);

	std::fstream tempFile; 		/* I have to declare it here, so it will be visible out of the scope. */
	bool tupleForInputAlreadyFound = false; 	/* I need to know if tuple was found, so I return true after rewriting whole file. */
	if (deleteAfterFind)
	{
		/* Create temporary file in case we need to remove line, so we can swap it later with the original one. */
		tempFile.open(TEMP_FILE.c_str(), std::ios::out);
		openFileInfo(tempFile);
	}

	bool tupleFinded = false;

	std::string line;
	while(getline(file, line).good()) /* Read file line by line */
	{
		++lineNum;
		std::cout << "Line: " << line << std::endl;

		int i = 0;
		bool isInString = false;

		Tuple tuple;
		std::string tupleElement;
		
		while(i < line.length())
		{
			while(line[i] != ' ' || isInString)
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
				
				++i;
			}

			std::cout << "Tuple element: " << tupleElement << std::endl;

			try 
			{
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

			tupleElement.clear();
			++i;
		}
		 
		t = tuple;
		tupleFinded = compareTupleWithTuplePattern(tuple, tuplePattern);

		if(tupleFinded && !tupleForInputAlreadyFound)
		{
			std::cout << "Tuple was found!" << std::endl;
			if (!deleteAfterFind)
			{
				file.close();
				return true;
			}
			else
			{
				tupleForInputAlreadyFound = true;
			}
		}
		else
		{
			std::cout << "Tuple was not found!" << std::endl;
			if (deleteAfterFind) 
			{
				tempFile << line << '\n';
			}
		}
	}


	file.close();
	if (deleteAfterFind) 
	{	
		tempFile.close();
		if (tupleForInputAlreadyFound)
		{
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

void getDataFromFile(TuplePattern tuplePattern, int timeout, bool isInputOperation)
{
	unsigned long lineNum = 0;
	Tuple tuple;

	if(findTuple(tuple, tuplePattern, lineNum, isInputOperation))
	{
		std::cout << std::endl;
		for(std::variant<int, float, std::string> element : tuple.tupleElements)
		{
			std::cout << std::get<0>(element) << " ";
		}

		std::cout << std::endl << "Number of line: " << lineNum << std::endl;
	}
}

void input(TuplePattern tuplePattern, int timeout)
{
	getDataFromFile(tuplePattern, timeout, true);
}


void read(TuplePattern tuplePattern, int timeout)
{
	getDataFromFile(tuplePattern, timeout, false);
}


int main()
{
	std::cout<<"UXP1A"<<std::endl;

	Tuple tuple;
	output(tuple);

	std::cout << std::endl;

	//Test parse function
	//TuplePattern tuplePattern = parsePattern("integer:*, float:>5.5, string:\"abc\""); /*std::string:*,*/

	//TuplePattern tuplePattern = parsePattern("integer:*, integer:>5, integer:<3");
	TuplePattern tuplePattern = parsePattern("string:<=\"EZTI\"");

	//TuplePattern tuplePattern = parsePattern("integer:*, integer:>5, float:<3, float:<=100.5");

	std::cout << std::endl;
	input(tuplePattern, 3);

	return 0;
}
